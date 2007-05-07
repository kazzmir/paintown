package com.rafkind.paintown;

import java.awt.event.WindowListener;
import java.awt.event.WindowEvent;

public class CloseHook implements WindowListener {

        private Lambda0 function;

        public CloseHook( Lambda0 _function ){
                function = _function;
        }

        public void windowActivated( WindowEvent e ){
        }

        public void windowClosed( WindowEvent e ){
        }

        public void windowClosing( WindowEvent e ){
		try{
			this.function.invoke();
		} catch ( Exception ex ){
		}
        }

        public void windowDeactivated( WindowEvent e ){
        }

        public void windowDeiconified( WindowEvent e ){
        }

        public void windowIconified( WindowEvent e ){
        }

        public void windowOpened( WindowEvent e ){
        }
}
