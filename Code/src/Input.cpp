#include "Input.hpp"

Input *Input::INSTANCE = NULL;
std::string Input::HISTORY_FILENAME = ".msgbd_history";

int ms_putchar(int c)
{
	write(STDOUT_FILENO, &c, 1);
	return (c);
}

Input *Input::getInstance(std::string prompt)
{
	if (!INSTANCE) INSTANCE = new Input(prompt);
	return INSTANCE;
}

Input::Input(std::string prompt) : PROMPT(prompt), CURSOR(0), CURRENT_HIST(-1)
{
	init();
	load_history();
}

Input::~Input()
{
	tputs(CAPS["reset"], 1, ms_putchar);
	tcsetattr(STDIN_FILENO, TCSANOW, &BACKUP);
}

void Input::init()
{
	const char *term;

	if (!(term = getenv("TERM")))
		term = std::string("xterm-256color").c_str();
	if (tgetent(NULL, term) <= 0)
	{
		GOOD = false;
		return ;
	}
	CAPS["le"] = tgetstr("le", NULL);
	CAPS["nd"] = tgetstr("nd", NULL);
	CAPS["dl"] = tgetstr("dl", NULL);
	CAPS["sc"] = tgetstr("sc", NULL);
	CAPS["rc"] = tgetstr("rc", NULL);
	CAPS["ch"] = tgetstr("ch", NULL);
	CAPS["up"] = tgetstr("up", NULL);
	CAPS["do"] = tgetstr("do", NULL);
	CAPS["reset"] = tgetstr("me", NULL);

	termios	term_attr;

	if (tcgetattr(STDIN_FILENO, &term_attr))
	{
		GOOD = false;
		return ;
	}
	if (tcgetattr(STDIN_FILENO, &BACKUP))
	{
		GOOD = false;
		return ;
	}
	term_attr.c_lflag &= ~(ICANON | ECHO);
	term_attr.c_cc[VMIN] = 1;
	term_attr.c_cc[VTIME] = 0;
	GOOD = !tcsetattr(STDIN_FILENO, TCSANOW, &term_attr);
}

bool Input::edit_line(char *command)
{
	if (command[0] == 0x7f && CURSOR > 0)
	{
		CURSOR--;
		BUFFER.erase(BUFFER.begin() + CURSOR);
		tputs(CAPS["le"], 1, ms_putchar);
	}
	else if (command[0] == '\n' || (command[0] == 4 && CURSOR == 0))
	{
		if (command[0] == '\n')
			write(1, "\n", 1);
		return false;
	}
	else
	{
		BUFFER.insert(BUFFER.begin() + CURSOR, command[0]);
		CURSOR++;
	}
	return true;
}

void Input::print_frame(char *command)
{
	if (CURSOR < BUFFER.length())
		tputs(CAPS["sc"], 1, ms_putchar);
	tputs(CAPS["dl"], 1, ms_putchar);
	write(STDOUT_FILENO, PROMPT.c_str(), PROMPT.length());
	write(STDOUT_FILENO, BUFFER.c_str(), BUFFER.length());
	if (CURSOR < BUFFER.length())
	{
		tputs(CAPS["rc"], 1, ms_putchar);
		if (command[0] != 0x7f)
			tputs(CAPS["nd"], 1, ms_putchar);
	}
}

std::string Input::getline()
{
	if (GOOD)
	{
		char COMMAND[16];
		ssize_t i;

		BUFFER.clear();
		CURSOR = 0;
		CURRENT_HIST = -1;
		OLD_BUFFER.clear();

		write(STDOUT_FILENO, PROMPT.c_str(), PROMPT.length());
		while ((i = read(STDIN_FILENO, COMMAND, 15)) > 0)
		{
			COMMAND[i] = 0;
			if (i > 1)
			{
				move_line(COMMAND);
				continue;
			}
			if ((BUFFER.length() == 0 && COMMAND[0] == 0x7f) ||
				(COMMAND[0] == 4 && CURSOR))
				continue ;
			else if (!edit_line(COMMAND))
				return BUFFER;
			print_frame(COMMAND);
		}
	}
	else
		std::cin >> BUFFER;
	return (BUFFER);
}

Input::SHORTCUTS Input::parse_arrow(char *command)
{
	command += 2;
	switch(*command)
	{
	case 'A':
		return UP;
	case 'B':
		return DOWN;
	case 'C':
		return RIGHT;
	case 'D':
		return LEFT;
	case 'H':
		return HOME;
	case 'F':
		return END;
	}
	if (*command != '1' && *(command + 1) != ';' && *(command + 2) != '5')
		return (DEFAULT);
	command += 3;
	if (*command == 'D')
		return (CTRL_LEFT);
	if (*command == 'C')
		return (CTRL_RIGHT);
	return (DEFAULT);
}

void Input::move_line(char *command)
{
	SHORTCUTS mode = parse_arrow(command);

	switch (mode)
	{
	case UP: case DOWN:
		loop_history(mode == DOWN);
		break;
	case HOME:
		cur_home();
		break;
	case END:
		cur_end();
		break;
	case CTRL_LEFT:
		ctrl_left();
		break;
	case CTRL_RIGHT:
		ctrl_right();
		break;
	case LEFT:
		if (CURSOR <= 0)
			break;
		tputs(CAPS["le"], 1, ms_putchar);
		CURSOR--;
		break ;
	case RIGHT:
		if (CURSOR >= BUFFER.length())
			break;
		tputs(CAPS["nd"], 1, ms_putchar);
		CURSOR++;
		break;
	default:
		break;
	}
}

void Input::cur_home()
{
	while (CURSOR > 0)
	{
		tputs(CAPS["le"], 1, ms_putchar);
		CURSOR--;
	}
}

void Input::cur_end()
{
	while (CURSOR < BUFFER.length())
	{
		tputs(CAPS["nd"], 1, ms_putchar);
		CURSOR++;
	}
}

void Input::ctrl_left()
{
	long pos = CURSOR;

	pos--;
	tputs(CAPS["le"], 1, ms_putchar);
	while (!isalnum(BUFFER[pos]) && pos < (int)BUFFER.length())
	{
		tputs(CAPS["le"], 1, ms_putchar);
		pos--;
	}
	while (isalnum(BUFFER[pos]) && pos < (int)BUFFER.length())
	{
		tputs(CAPS["le"], 1, ms_putchar);
		pos--;
	}

	do tputs(CAPS["nd"], 1, ms_putchar); while (++pos < 0);

	if (!PROMPT.length() && !pos)
		tputs(CAPS["le"], 1, ms_putchar);
	CURSOR = pos;
}

void Input::ctrl_right()
{
	while (isalnum(BUFFER[CURSOR]) && CURSOR < BUFFER.length())
	{
		tputs(CAPS["nd"], 1, ms_putchar);
		CURSOR++;
	}
	while (!isalnum(BUFFER[CURSOR]) && CURSOR < BUFFER.length())
	{
		tputs(CAPS["nd"], 1, ms_putchar);
		CURSOR++;
	}
	while (CURSOR > BUFFER.length())
	{
		CURSOR--;
		tputs(CAPS["le"], 1, ms_putchar);
	}
}

void Input::load_history()
{
	std::fstream file(Input::HISTORY_FILENAME, std::ios::in);
	std::string line;

	if (file.is_open())
	{
		file.seekg(0, std::ios::beg);
		while (std::getline(file, line))
			if (!line.empty())
				HISTORY.push_back(line);
		file.close();
	}
}

void Input::loop_history(bool forward)
{
	static bool useHist = false;
	if (!forward)
		CURRENT_HIST = (CURRENT_HIST == -1) ? HISTORY.size() - 1 : std::max(CURRENT_HIST - 1, 0);
	else if (CURRENT_HIST != -1)
		CURRENT_HIST++;
	if (CURRENT_HIST >= (int)HISTORY.size())
		CURRENT_HIST = -1;
	if (CURRENT_HIST == -1 && useHist)
	{
		BUFFER = OLD_BUFFER;
		OLD_BUFFER.clear();
		useHist = false;
	}
	else if (CURRENT_HIST != -1)
	{
		if (!useHist && OLD_BUFFER.empty())
			OLD_BUFFER = BUFFER;
		else if (BUFFER != OLD_BUFFER)
			BUFFER.clear();
		BUFFER = HISTORY[CURRENT_HIST];
		useHist = true;
	}
	CURSOR = BUFFER.length();
	tputs(CAPS["dl"], 1, ms_putchar);
	write(STDOUT_FILENO, PROMPT.c_str(), PROMPT.length());
	write(STDOUT_FILENO, BUFFER.c_str(), BUFFER.length());
}

void Input::addCommand(std::string command)
{
	if (command.empty())
		return ;
	HISTORY.push_back(command);
}

void Input::saveHistory() const
{
	std::fstream file(HISTORY_FILENAME, std::ios::out | std::ios::trunc);

	if (!file.is_open())
		return ;
	for (std::string str : HISTORY)
		file << str << std::endl;
	file.close();
}
