#ifndef INPUT_HPP
#define INPUT_HPP

#include <term.h>
#include <termcap.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <termio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Input
{
private:
	enum SHORTCUTS
	{
		DEFAULT = -1,
		UP = 0,
		DOWN = 1,
		RIGHT = 2,
		LEFT = 3,
		HOME = 4,
		END = 5,
		CTRL_LEFT = 6,
		CTRL_RIGHT = 7
	};

	typedef struct termios termios;

	static Input* INSTANCE;
	static std::string HISTORY_FILENAME;

	std::string PROMPT;
	std::string BUFFER;
	size_t CURSOR;
	std::string OLD_BUFFER;
	bool GOOD;
	termios BACKUP;
	int CURRENT_HIST;

	std::vector<std::string> HISTORY;
	std::map<std::string, char *> CAPS;

	Input(std::string prompt);

	void init();
	bool edit_line(char *command);
	void move_line(char *command);
	void cur_home();
	void cur_end();
	void ctrl_left();
	void ctrl_right();
	void print_frame(char *command);
	SHORTCUTS parse_arrow(char *command);

	void load_history();
	void loop_history(bool forward);
public:
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	~Input();

	static Input *getInstance(std::string prompt = "");
	std::string getline();
	void saveHistory() const;
	void addCommand(std::string command);
};

#endif
