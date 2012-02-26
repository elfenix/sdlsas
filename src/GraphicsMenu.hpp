#ifndef MENU_H_
#define MENU_H_

#include <vector>
#include <string>
#include "ScreenBitmap.hpp"

class GraphicsMenu
{
public:
	typedef void (*GraphicsMenuAction)(int* done);

public:
	GraphicsMenu(ScreenBitmap* mouse, ScreenBitmap* backdrop,
			const std::vector< std::string >& textEntries);
	virtual ~GraphicsMenu();

	void run_menu();

	void setAction(int j, GraphicsMenuAction action);

	void setTopPad(float y);
	void enableSound(int num);

	void setMinSelectable(int min);
	void setMaxSelectable(int max);

private:
    int get_mouse_selection(float p_x, float p_y );
    void get_mouse_state( float& p_x, float& p_y );

private:
	std::vector<ScreenBitmap>		m_menu_strings;
	std::vector<ScreenBitmap>		m_highlight_strings;
	std::vector<GraphicsMenuAction>	m_actions;

	ScreenBitmap* m_mouse;
	ScreenBitmap* m_backdrop;
	SDL_Rect* m_activeRegions;

	float m_topPad;

	int m_minSelect;
	int m_maxSelect;

	int m_num_entries;
	int m_soundOn;
	float m_menu_height;
};

#endif
