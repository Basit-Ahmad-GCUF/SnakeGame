#include <iostream>
#include <deque>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "utilities.h"



	static const char EMPTY      = ' ';
	static const char SNAKE_HEAD = '@';
	static const char SNAKE_BODY = 'O';
	static const char FOOD_SMALL = '*';
	static const char FOOD_LARGE = '$';


struct point {
	int x,y;
};
struct record {
	int score;
	std::string name;
};
enum Direction {
	up,
	down,
	left,
	right
};

class player {
	private:
		int score;
		std::string name;
	public:
		player(std::string n, int s) : name(n) , score(s) {}
		std::string getname() { return name; }
		int getscore() { return score; }
		void setname(std::string n) { this->name = n; }
		void setscore(int s) { this->score = s; }
		void addscore(int s) { score += s; }
};
class snake {
	private:
		Direction direction;
		std::deque<point> body;
		int size;
		bool alive;
		bool food_eaten;
	public:
		snake() {
			direction = down;
			alive = true;
			food_eaten = false;
			size = 3;
			body = { {3,3},{3,4},{3,5} };
		}
		bool check_status() {
			return alive;
		}
		void ate_food(bool food_eaten) { this->food_eaten = food_eaten; }
		void set_status(bool state) {	
			alive = state;
		}
		void set_direction(char input){
		    if(input == 'w' || input =='W') direction = up;
		    else if(input == 's' || input =='S') direction = down;
		    else if(input == 'a' || input =='A') direction = left;
		    else if(input == 'd' || input =='D') direction = right;
		}
		void movement() {
			point new_head;
			if(direction == left){
				new_head.x = body.front().x;
				new_head.y = body.front().y - 1;
			}else if(direction == right){
				new_head.x = body.front().x;
				new_head.y = body.front().y + 1;
			}else if(direction == up){
				new_head.x = body.front().x - 1;
				new_head.y = body.front().y;
			}else if(direction == down){
				new_head.x = body.front().x + 1;
				new_head.y = body.front().y;
			}
			
			body.push_front(new_head);
			
			if(food_eaten == false){
				body.pop_back();
			}else{
				food_eaten = false;
			}
			
		}
		std::deque<point> get_body() { return body; }
};
class board{
	private:
		char grid[20][50];
	public:
		void intitialize_grid(){
			for(int x=0;x<20;x++)
				for(int y=0;y<50;y++)
					grid[x][y] = EMPTY;
		}
		void set_point(point p, char c){
			grid[p.x][p.y] = c;
		}
		char get_symbol_atpoint(point p) { return grid[p.x][p.y]; }
		void show_grid(std::string name, int score){
			std::cout<<"\n\t\t\t+"; design::line(50,"="); std::cout<<"+";
			std::cout<<"\n\t\t\t| "<<std::left<<std::setw(20)<<name<<": "<<std::setw(5)<<score; design::line(22," "); std::cout<<"|";
			std::cout<<"\n\t\t\t+"; design::line(50,"="); std::cout<<"+";
			
			for(int x = 0 ; x < 20 ; x++){
				std::cout<<"\n\t\t\t|";
				for(int y = 0 ; y < 50 ; y++){
					std::cout<<grid[x][y];
				}
				std::cout<<"|";
			}
			
			std::cout<<"\n\t\t\t+"; design::line(50,"="); std::cout<<"+";
		}
};
class game_manager{
	private:
		int game_speed = 100;
		player *p;
		snake *snk;
		board brd;
		point food_position;
		char symbol;
	public:
		void get_player(player &p){ this->p = &p; }
		bool check_player(){
			if(p != NULL)
				return true;
			else
				return false;
		}
		void spawn_food(){
			//snk->ate_food(false);
			point p;
			p.x = rand() % 20;
			p.y = rand() % 50;
			int spawn_control = rand() % 100; 
			if(spawn_control > 1 && spawn_control < 20){
				brd.set_point(p, FOOD_LARGE);
				food_position = p;
				symbol = FOOD_LARGE;
			}else{
				brd.set_point(p, FOOD_SMALL);
				food_position = p;
				symbol = FOOD_SMALL;
			}
		}
		void start_round(){
			brd.intitialize_grid();
			char input;
			
			if(!check_player()){
				std::cout<<color::red<< style::blink <<"\n\t\t\t [WARNING] Player Not Found!";
				return;
			}
			int j = 1;
			while(snk->check_status()){
				int i = 1;
				if(_kbhit()){ input = _getch(); snk->set_direction(input); }
				std::cout << "\033[H"; // Moving the Cursor to Start.
				//==============================================
				
				brd.intitialize_grid();
				if(j==1) { spawn_food(); }
				brd.set_point(food_position, symbol);
				for(point p : snk->get_body()) {
					if(i == 1) brd.set_point(p, SNAKE_HEAD);
					else brd.set_point(p, SNAKE_BODY);
					i++;
				}
				snk->movement();
				
				point head = snk->get_body().front();
				char cell = brd.get_symbol_atpoint(head);
				if(cell == FOOD_LARGE){
					snk->ate_food(true);
					p->addscore(10);
					spawn_food();
				}else if(cell == FOOD_SMALL){
					snk->ate_food(true);
					p->addscore(2);
					spawn_food();
				}
				if(head.x < 0 || head.x == 20 || head.y < 0 || head.y == 50) { snk->set_status(false); }
				if(cell == SNAKE_BODY) { snk->set_status(false); }
				brd.show_grid(p->getname(),p->getscore());
				
				//==============================================
				std::this_thread::sleep_for(std::chrono::milliseconds(game_speed));
				j++;
			}
			std::cout << "\033[H"; std::cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\t\t\t\t\t    "<<style::blink<<color::red; sys::type_write("GAME OVER!");
			std::cout <<color::reset<<"\n\t\t\t\t\t"<<color::b_green; sys::type_write("Your Final Score: "); std::cout<<p->getscore();
			std::cout <<"\n\n\n\n\n\n\n\n\n\n"<<color::reset;
		}
		void mainmenu() {
			char difficulty;
			std::string difficult;
			std::string new_name;
			char ch = ' ';
			do {
				if(game_speed == 200) difficult = "Easy";
				else if(game_speed == 100) difficult = "Medium";
				else if(game_speed == 50) difficult = "Hard";
				else if(game_speed == 25) difficult = "Hardcore";
				
				sys::clearscreen();
				std::cout<<color::reset<<color::b_cyan;
				design::draw_header("      MAIN MENU      ", "=");
				std::cout<<color::b_white;
				std::cout<<"\n\t\t\t||                                         ||";
				std::cout<<"\n\t\t\t|| "<<color::b_yellow<<" [1] "<<style::italic<<"Start a New Round.                 "<<color::reset<<color::b_white<<"||";
				std::cout<<"\n\t\t\t||                                         ||";
				std::cout<<"\n\t\t\t|| "<<color::b_yellow<<" [2] "<<style::italic<<"Set New Player Name.               "<<color::reset<<color::b_white<<"||";
				std::cout<<"\n\t\t\t||                                         ||";
				std::cout<<"\n\t\t\t|| "<<color::b_yellow<<" [3] "<<style::italic<<"Set Difficulty.                    "<<color::reset<<color::b_white<<"||";
				std::cout<<"\n\t\t\t||                                         ||";
				std::cout<<"\n\t\t\t|| "<<color::b_yellow<<" [4] "<<style::italic<<"Exit the Game.                     "<<color::reset<<color::b_white<<"||";
				std::cout<<"\n\t\t\t||                                         ||";
				std::cout<<"\n\t\t\t++=========================================++";
				std::cout<<"\n\t\t\t||"<<color::b_yellow<<" Current Player: "<<color::b_green<<std::left<<std::setw(24)<<p->getname()<<color::b_white<<"||";
				std::cout<<"\n\t\t\t||"<<color::b_yellow<<" Difficulty    : "<<color::b_green<<std::left<<std::setw(24)<<difficult<<color::b_white<<"||";
				std::cout<<"\n\t\t\t++=========================================++";
					
				ch = _getch();
				switch(ch) {
					case '1':
						sys::clearscreen();
						snk = new snake;
						p->setscore(0);
						start_round();
						delete snk;
						break;
					case '2':
						sys::clearscreen();
						std::cout<<color::b_yellow<<"\n\t\t\t===============================================";
						std::cout<<color::b_white<<"\n\n\t\t\t > Enter New Player Name : "<<color::b_green;
						getline(std::cin, new_name);
						p->setname(new_name);
						std::cout<<color::b_white;
						break;
					case '3':
						do{
							sys::clearscreen();
							std::cout<<color::b_yellow<<"\t\t\tSELECT NEW DIFFICULTY\n\t\t\t===============================================";
							std::cout<<color::b_white<<"\n\n\t\t\t > Enter New Difficulty : \n\t\t\t\t 1. Easy \n\t\t\t\t 2. Medium \n\t\t\t\t 3. Hard \n\t\t\t\t 4. Hardcore"<<color::b_green;
							difficulty = _getch();
							if(difficulty == '1') game_speed = 200;
							else if(difficulty == '2') game_speed = 100;
							else if(difficulty == '3') game_speed = 50;
							else if(difficulty == '4') game_speed = 25;
						}while(difficulty != '1' && difficulty != '2' && difficulty != '3' && difficulty != '4');
						break;
					case '4':
						sys::clearscreen();
						std::cout<<"\n\n\t\t\t > Exiting.";
						sys::printdots(3);
						break;
					default:
						std::cout<<" [ERROR] Wrong Input !";		
				}
			}while(ch != '4');
			
		}
};

//		std::cout<<"\n\t\t\t";

int main() {
	
	std::cout << "\033[?25l"; // Hides the Cursor Entirely
	srand(time(0));
	game_manager GM;
	player P1("Player (* - *)", 0);
	GM.get_player(P1);
	
	GM.mainmenu();
	
	sys::clearscreen();
	design::draw_header("Thank You For Playing!","=");
	std::cout<<"\n\t\t\t\t> Game Created By (0% AI):";
 	std::cout<<color::green<<style::bold<<style::italic<<style::underline<<style::blink<<"\n\t\t\t\tBASIT AHMAD"<<color::reset<<std::endl;
 	return 0;
}
