#ifndef _paintown_menu_item_h
#define _paintown_menu_item_h

class MenuItem
{
	public:
		// Do whatever is needed for the requested item
		virtual void act()=0;

		MenuItem();
	
		virtual ~MenuItem();
		
	private:
		// Nothing
};

#endif
