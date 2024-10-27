#include "src/include/demon.h"


int main(int argc, char* argv[]) {

    Demon demon;
		
    while (!demon.engine.win->is_closed())
        demon.update();
	
    return 0;
}
