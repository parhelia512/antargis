#include <iostream>
#include <ag_main.h>
#include <ag_video.h>
#include <ag_application.h>
#include <ant_intro_app.h>
#include <ant_menu_app.h>
#include <ag_layout.h>
#include <ant_game_app.h>
#include <ag_rand.h>

/**
 * Main entry for the game. main should get wrapped within SDL_main on windows automatically.
 */ 
int main ( int argc, char **argv ) {
    setDebugLevel(0);

    AGMain main;
    main.setVideo ( new AGVideoManager() );
    main.setRand(new AGRandomizer("abc"));

    main.getVideo()->initVideo ( 1024,768,32,false,true );
    AGLayout::registerLayouts();

    if ( argc>1 ) {

      AntGameApp app ( getVideo()->width(),getVideo()->height() );
      // continue saved game (for testing purpose)
      if(AGString(argv[1])=="c")
        app.init("tempSaved");
      else if(AGString(argv[1])=="x")
        app.init("data/levels/tutorial/tutorial0.antlvl");
      else
        // run specific level
        app.init(argv[1]);
      app.run();
      return 0;
    }

    // run intro
    AntIntroApp intro;
    intro.init();
    intro.run();

    // run main menu, which then runs other 'apps'
    AntMenuApp menu;
    menu.init();
    menu.run();

    return 0;
}
