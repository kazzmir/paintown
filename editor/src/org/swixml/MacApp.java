/*--
 $Id: MacApp.java,v 1.2 2004/10/05 21:32:35 tichy Exp $

 Copyright (C) 2003-2004 Wolf Paulus.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions, and the disclaimer that follows
 these conditions in the documentation and/or other materials provided
 with the distribution.

 3. The end-user documentation included with the redistribution,
 if any, must include the following acknowledgment:
        "This product includes software developed by the
         SWIXML Project (http://www.swixml.org/)."
 Alternately, this acknowledgment may appear in the software itself,
 if and wherever such third-party acknowledgments normally appear.

 4. The name "Swixml" must not be used to endorse or promote products
 derived from this software without prior written permission. For
 written permission, please contact <info_AT_swixml_DOT_org>

 5. Products derived from this software may not be called "Swixml",
 nor may "Swixml" appear in their name, without prior written
 permission from the Swixml Project Management.

 THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED.  IN NO EVENT SHALL THE SWIXML PROJECT OR ITS
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 ====================================================================

 This software consists of voluntary contributions made by many
 individuals on behalf of the Swixml Project and was originally
 created by Wolf Paulus <wolf_AT_swixml_DOT_org>. For more information
 on the Swixml Project, please see <http://www.swixml.org/>.
*/
package org.swixml;

import com.apple.eawt.*;
import javax.swing.*;

import java.awt.event.ActionEvent;
import java.util.Map;

/**
 * A MacApp object acompanies a SwingEngine generated UI, when run on a Mac.
 * The MacApp dispatches MacMenu requests to the registered actions.<BR>
 * It remains in the responsibility of the main application that the registered
 * Action are available and fully funtional (loaded).
 */
public class MacApp extends Application {
  private static MacApp INSTANCE = null;

  public synchronized static MacApp getInstance() {
    if (INSTANCE == null) {
      INSTANCE = new MacApp();
    }
    return INSTANCE;
  }

  //////////////////////////////////////////////////

  Action aboutAction = null;
  Action fileAction = null;
  Action appAction = null;
  Action prefAction = null;
  Action printAction = null;
  Action reopenAction = null;
  Action quitAction = null;
int sequence;


  private MacApp() {
      this.sequence = 0;
    addApplicationListener( new ApplicationAdapter() {

      public void handleAbout( ApplicationEvent event ) {
        if (aboutAction != null) {
          event.setHandled( true );         
          aboutAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_ABOUT ));
        } else {
          super.handleAbout( event );
        }
      }

      public void handleOpenApplication( ApplicationEvent event ) {
        if (appAction != null) {
          appAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_OPENAPP) );
          event.setHandled( true );
        } else {
          super.handleOpenApplication( event );
        }
      }

      public void handleOpenFile( ApplicationEvent event ) {
        if (fileAction != null) {
          fileAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_OPENFILE) );
          event.setHandled( true );
        } else {
          super.handleOpenFile( event );
        }
      }

      public void handlePreferences( ApplicationEvent event ) {
        if (prefAction != null) {
          prefAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_PREF) );
          event.setHandled( true );
        } else {
          super.handlePreferences( event );
        }
      }

      public void handlePrintFile( ApplicationEvent event ) {
        if (printAction != null) {
          printAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_PRINT) );
          event.setHandled( true );
        } else {
          super.handlePrintFile( event );
        }
      }

      public void handleQuit( ApplicationEvent event ) {
        if (quitAction != null) {
          quitAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_QUIT) );
          event.setHandled( true );
        } else {
          super.handleQuit( event );
        }
      }

      public void handleReOpenApplication( ApplicationEvent event ) {
        if (reopenAction != null) {
          reopenAction.actionPerformed( new ActionEvent(event.getSource(),
                  MacApp.this.sequence++,
                  Parser.ATTR_MACOS_OPENAPP) );
          event.setHandled( true );
        } else {
          super.handleReOpenApplication( event );
        }
      }

    } );
  }


  public void update( Map action_map ) {

    if (action_map.containsKey( Parser.ATTR_MACOS_ABOUT )) {
      aboutAction = (Action) action_map.get( Parser.ATTR_MACOS_ABOUT );
      this.setEnabledAboutMenu( aboutAction != null );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_PREF )) {
      prefAction = (Action) action_map.get( Parser.ATTR_MACOS_PREF );
      this.setEnabledPreferencesMenu( prefAction != null );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_OPENAPP )) {
      appAction = (Action) action_map.get( Parser.ATTR_MACOS_OPENAPP );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_REOPEN )) {
      reopenAction = (Action) action_map.get( Parser.ATTR_MACOS_REOPEN );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_OPENFILE )) {
      fileAction = (Action) action_map.get( Parser.ATTR_MACOS_OPENFILE );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_PRINT )) {
      printAction = (Action) action_map.get( Parser.ATTR_MACOS_PRINT );
    }
    if (action_map.containsKey( Parser.ATTR_MACOS_QUIT )) {
      quitAction = (Action) action_map.get( Parser.ATTR_MACOS_QUIT );
    }
  }
}

