#include "sasteroids.h"

GraphicsMenu::GraphicsMenu(SBitmap* tmouse, SBitmap* tback, char** text)
{
  int text_size, j;


  for(text_size = 0; text[text_size]; text_size++);
  menu_strings = new SBitmap[text_size];
  highlight_strings = new SBitmap[text_size];
  activeRegions = new SDL_Rect[text_size];
  actions = new GraphicsMenuAction[text_size];

  
  for(j = 0, menu_height = 0.0f; j < text_size; j++) {
    SDL_Surface* tmp;

    tmp = Ui::get_text(text[j], 255, 255, 255);
    menu_strings[j].LoadSurface(tmp);
    menu_height += menu_strings[j].height();
    SDL_FreeSurface(tmp);

    tmp = Ui::get_text(text[j], 0, 255, 255);
    highlight_strings[j].LoadSurface(tmp);
    SDL_FreeSurface(tmp);

    actions[j] = 0;
  }

  backdrop = tback;
  mouse = tmouse;
  topPad = 0.0f;
  minSelect = 0;
  maxSelect = text_size - 1;
  soundOn = 0;

  num_entries = text_size;
}


void GraphicsMenu::setTopPad(float y) 
{
  topPad = y;
}


void GraphicsMenu::setAction(int j, GraphicsMenuAction action)
{
  if(j > minSelect && j <= maxSelect) actions[j] = action;

}


void GraphicsMenu::setMinSelectable(int min) 
{
  minSelect = min;
}


void GraphicsMenu::setMaxSelectable(int max)
{
  maxSelect = max;
}


void GraphicsMenu::enableSound(int soundNum)
{
  soundOn = soundNum;
}

void GraphicsMenu::RunMenu() 
{
  int done = 0, dirty = 0, playsound = 0, tX, tY;
  int j, execute = 0;
  SDL_Event event;
  float start_height, start_width;
  float mouseX, mouseY;

  int selected = minSelect;
  
  // Setup mouse areas
  if(activeRegions) {
    start_height = float(Ui::HEIGHT() >> 1);
    start_height += menu_height / 2.0f; 
    start_height -= topPad;
    
    for(j = 0; j < num_entries; j++) {

      start_width = float(Ui::WIDTH() >> 1);
      start_width -= menu_strings[j].width() / 2.0f;

      activeRegions[j].x = int(start_width);
      activeRegions[j].y = int(start_height-menu_strings[j].height());
      activeRegions[j].w = int(menu_strings[j].width());
      activeRegions[j].h = int(menu_strings[j].height());

      start_height = start_height - menu_strings[j].height();
    }

  }

  SDL_GetMouseState(&tX, &tY);
  mouseX = float(tX);
  mouseY = float(tY);

  while(!done) {
    execute = 0;
    while(SDL_PollEvent(&event)) {
      switch(event.type) 
	{
	case SDL_MOUSEMOTION:
	  mouseX = event.motion.x;
	  mouseY = event.motion.y;

	  if(activeRegions) {
	    int j, newselect = -1;

	    for(j = minSelect; j <= maxSelect; j++) {
	      if(mouseX > float(activeRegions[j].x) && 
		 (float(Ui::HEIGHT()) - mouseY) > float(activeRegions[j].y) &&
		 mouseX < float(activeRegions[j].x + activeRegions[j].w) &&
		 (float(Ui::HEIGHT()) - mouseY) < float(activeRegions[j].y + activeRegions[j].h))
		newselect = j;
	    }

	    if(selected != newselect && newselect != -1) {
	      selected = newselect;
	      playsound = 1;
	    }

	  }
	  break;

	case SDL_MOUSEBUTTONUP:

	  // Check button down, mouse must be in active area ...
	  if(event.button.button == SDL_BUTTON_LEFT) {
	    int j, newselect = -1;


	    for(j = minSelect; j <= maxSelect; j++) {
	      if(mouseX > float(activeRegions[j].x) && 
		 (float(Ui::HEIGHT()) - mouseY) > float(activeRegions[j].y) &&
		 mouseX < float(activeRegions[j].x + activeRegions[j].w) &&
		 (float(Ui::HEIGHT()) - mouseY) < float(activeRegions[j].y + activeRegions[j].h))
		newselect = j;
	    }
	    
	    if(selected == newselect && newselect != -1) {
	      execute = 1;
	    }
	  }
	  break;
	  
	case SDL_QUIT:
	  exit(0);
	  break;

	case SDL_KEYDOWN:
	  if(event.key.keysym.sym == SDLK_UP) { playsound = 1; selected--; }
	  if(event.key.keysym.sym == SDLK_DOWN) { playsound = 1; selected++; }
	  if(event.key.keysym.sym == SDLK_RETURN) { execute = 1; }

	  if(selected < minSelect) selected = maxSelect;
	  if(selected > maxSelect) selected = minSelect;
	  break;
	}
    }

    if(playsound) {
      if(soundOn) PlaySound(soundOn);
      playsound = 0;
    }


    if(execute) {
      if(actions[selected])
	(actions[selected])(&done);
      execute = 0;
    }


    Ui::predraw();
      
    start_height = float(Ui::HEIGHT() >> 1);
    start_height += menu_height / 2.0f; 
    start_height -= topPad;
      
    backdrop->putA(0.0f, float(Ui::HEIGHT()));
    
    
    for(j = 0; j < num_entries; j++) {
      start_width = float(Ui::WIDTH() >> 1);
      start_width -= menu_strings[j].width() / 2.0f;
      menu_strings[j].putA(start_width, start_height);
      if(j == selected) highlight_strings[j].putA(start_width-1.0f, start_height-1.0f);
      start_height = start_height - menu_strings[j].height();
    }
    
    mouse->put(mouseX, float(Ui::HEIGHT()) - mouseY);
    Ui::updateScreen();
  }
}
