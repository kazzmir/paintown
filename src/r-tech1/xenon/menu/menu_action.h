#ifndef _rtech1_menu_action_h
#define _rtech1_menu_action_h

class MenuAction
{
	public:
		// Do whatever is needed for the requested item
		virtual void act()=0;

		MenuAction();
	
		virtual ~MenuAction();
		
	private:
		// Nothing
};

#endif
