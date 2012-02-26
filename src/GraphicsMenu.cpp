#include "GraphicsMenu.hpp"
#include "ui.h"

GraphicsMenu::GraphicsMenu(ScreenBitmap* tmouse, ScreenBitmap* tback, const std::vector< std::string >& text ) :
	m_menu_strings( text.size() ),
	m_highlight_strings( text.size() ),
	m_actions( text.size() )
{
	unsigned int j;
	m_activeRegions = new SDL_Rect[text.size()];

	for ( j = 0, m_menu_height = 0.0f; j < text.size(); j++)
	{
		SDL_Surface* tmp;

		tmp = Ui::get_text(text[j].c_str(), 255, 255, 255);
		m_menu_strings[j].load_surface(tmp);
		m_menu_height += m_menu_strings[j].height();
		SDL_FreeSurface(tmp);

		tmp = Ui::get_text(text[j].c_str(), 0, 255, 255);
		m_highlight_strings[j].load_surface(tmp);
		SDL_FreeSurface(tmp);

		m_actions[j] = 0;
	}

	m_backdrop = tback;
	m_mouse = tmouse;
	m_topPad = 0.0f;
	m_minSelect = 0;
	m_maxSelect = text.size() - 1;
	m_soundOn = 0;

	m_num_entries = text.size();
}


GraphicsMenu::~GraphicsMenu()
{
	delete[] m_activeRegions;
}


void GraphicsMenu::setTopPad(float y) 
{
	m_topPad = y;
}


void GraphicsMenu::setAction(int j, GraphicsMenuAction action)
{
	if(j > m_minSelect && j <= m_maxSelect)
		m_actions[j] = action;
}


void GraphicsMenu::setMinSelectable(int min) 
{
	m_minSelect = min;
}


void GraphicsMenu::setMaxSelectable(int max)
{
	m_maxSelect = max;
}


void GraphicsMenu::enableSound(int soundNum)
{
	m_soundOn = soundNum;
}


int GraphicsMenu::get_mouse_selection(float p_x, float p_y )
{
	int new_selection = -1;
	float y = (float(Ui::HEIGHT() - p_y));

	for( int j = m_minSelect;j <= m_maxSelect;j++)
	{
		if( p_x > float(m_activeRegions[j].x) &&
			y > float(m_activeRegions[j].y) &&
			p_x < float(m_activeRegions[j].x + m_activeRegions[j].w) &&
			y < float(m_activeRegions[j].y + m_activeRegions[j].h))
		{
			new_selection = j;
		}
    }

    return new_selection;
}


void GraphicsMenu::get_mouse_state( float& p_x, float& p_y )
{
	int x, y;

	SDL_GetMouseState( &x, &y );
	p_x = float( x );
	p_y = float( y );
}


void GraphicsMenu::run_menu() 
{
	int done = 0, playsound = 0, tX, tY;
	int j;
	SDL_Event event;
	float start_height, start_width;
	float mouseX, mouseY;

	int selected = m_minSelect;
  
	// Setup mouse areas
	start_height = float(Ui::HEIGHT() >> 1);
	start_height += m_menu_height / 2.0f;
	start_height -= m_topPad;
    
	for(j = 0; j < m_num_entries; j++)
	{
		start_width = float(Ui::WIDTH() >> 1);
		start_width -= m_menu_strings[j].width() / 2.0f;

		m_activeRegions[j].x = int(start_width);
		m_activeRegions[j].y = int(start_height-m_menu_strings[j].height());
		m_activeRegions[j].w = int(m_menu_strings[j].width());
		m_activeRegions[j].h = int(m_menu_strings[j].height());

		start_height = start_height - m_menu_strings[j].height();
	}

	get_mouse_state( mouseX, mouseY );


	while(!done)
	{
		int new_selection = selected;
		bool run_action = false;

		while( SDL_PollEvent(&event) )
		{
			switch( event.type )
			{
			case SDL_QUIT:
				exit(0);
				break;

			case SDL_MOUSEMOTION:
				mouseX = event.motion.x;
				mouseY = event.motion.y;
			    new_selection = get_mouse_selection( mouseX, mouseY );
				break;

			case SDL_MOUSEBUTTONUP:
				// Check button down, mouse must be in active area ...
				if (event.button.button == SDL_BUTTON_LEFT)
				{
					new_selection = get_mouse_selection( mouseX, mouseY );
					if( new_selection > -1 && selected == new_selection )
						run_action = true;
				}
				break;

			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_UP)
				{
					new_selection = selected - 1;
				}
				if (event.key.keysym.sym == SDLK_DOWN)
				{
					new_selection = selected + 1;
				}
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					run_action = true;
				}

				if (new_selection < m_minSelect)
					new_selection = m_maxSelect;
				if (new_selection > m_maxSelect)
					new_selection = m_minSelect;
				break;

			default:
				break;
			}
		}

		if( (new_selection > -1 && new_selection != selected) || run_action )
		{
			selected = new_selection;

			if( m_soundOn )
				PlaySound(m_soundOn);
		}

		if( run_action )
		{
			if( m_actions[selected] )
			{
				(m_actions[selected])(&done);
			}

			get_mouse_state( mouseX, mouseY );
		}

		Ui::predraw();

		start_height = float(Ui::HEIGHT() >> 1);
		start_height += m_menu_height / 2.0f;
		start_height -= m_topPad;

		m_backdrop->draw_alpha(0.0f, float(Ui::HEIGHT()));

		for (j = 0; j < m_num_entries; j++)
		{
			start_width = float(Ui::WIDTH() >> 1);
			start_width -= m_menu_strings[j].width() / 2.0f;
			m_menu_strings[j].draw_alpha(start_width, start_height);
			if (j == selected)
				m_highlight_strings[j].draw_alpha(start_width - 1.0f,
						start_height - 1.0f);
			start_height = start_height - m_menu_strings[j].height();
		}

		m_mouse->draw(mouseX, float(Ui::HEIGHT()) - mouseY);
		Ui::updateScreen();
	}
}
