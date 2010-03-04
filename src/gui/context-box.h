#ifndef _paintown_gui_context_box_h
#define _paintown_gui_context_box_h

#include <string>

#include "gui/widget.h"

namespace Gui{

template <class T>
class ContextBox : public Widget{
	public:
		ContextBox();
		ContextBox( const Box & b );
		virtual ~ContextBox();
		
		//! copy
		ContextBox &operator=( const ContextBox &);
		
		//! Logic
		virtual void act();
		
		//! Render
		virtual void render(const Bitmap & work);

        //! Next
        virtual void next();

        //! Previous
        virtual void previous();

        //! Set context list
        virtual inline void setList(std::vector<T> & list){
            this->context = &list;
        }
        //! Get current index
        virtual inline int getCurrentIndex(){
            return this->current;
        }
        //! Get current item
        virtual inline T getCurrentItem(){
            return this->context[this->current];
        }
    private: 
        //! Current index
        unsigned int current;

        //! Context list
        std::vector<T> * context;

}

#endif
