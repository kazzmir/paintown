/*--
 $Id: XDialog.java,v 1.2 2004/08/20 05:59:30 wolfpaulus Exp $

 Copyright (C) 2003-2007 Wolf Paulus.
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

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

/**
 * XDialog simply extends JDialog to allow instantiation with a parent frame
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.2 $
 */
public class XDialog extends JDialog {
  /**
   * Creates a non-modal dialog without a title and without a specified
   * <code>Frame</code> owner.  A shared, hidden frame will be
   * set as the owner of the dialog.
   * <p>
   * This constructor sets the component's locale property to the value
   * returned by <code>JComponent.getDefaultLocale</code>.
   * </p>
   * <p>
   * setLocationRelativeTo is called for the instanced dialog if a parent
   * could be provided by the SwingEngine.
   * </p>
   * @exception HeadlessException if GraphicsEnvironment.isHeadless()
   * returns true.
   * @see GraphicsEnvironment#isHeadless
   * @see JComponent#getDefaultLocale
   * @see Window#setLocationRelativeTo
   */
  public XDialog() throws HeadlessException {
    super( SwingEngine.getAppFrame() != null && SwingEngine.getAppFrame().isDisplayable() ? SwingEngine.getAppFrame() : null );
  }

  /**
   * Makes the Dialog visible. If the dialog and/or its owner
   * are not yet displayable, both are made displayable.  The
   * dialog will be validated prior to being made visible.
   * If the dialog is already visible, this will bring the dialog
   * to the front.
   * <p>
   * If the dialog is modal and is not already visible, this call will
   * not return until the dialog is hidden by calling <code>hide</code> or
   * <code>dispose</code>. It is permissible to show modal dialogs from
   * the event dispatching thread because the toolkit will ensure that
   * another event pump runs while the one which invoked this method
   * is blocked.
   * @see Component#hide
   * @see Component#isDisplayable
   * @see Component#validate
   * @see Dialog#isModal
   */
  @Override
  @Deprecated
  public void show() {
    this.setLocationRelativeTo( SwingUtilities.windowForComponent( this ) );
    super.show();
  }

  /**
   * Sets the application frame system icon.
   * <pre><b>Note:</b><br>
   * The provided icon is only applied if an enclosing frame doesn't really exists yet or does not have an icon set.
   * </pre>
   * @param image <code>Image</code> the image to become the app's system icon.
   */
  public synchronized void setIconImage( Image image ) {
    Frame f = JOptionPane.getFrameForComponent( this );
    if (f != null && f.getIconImage() == null) {
      f.setIconImage( image );
    }
  }

  /**
   * Overwrites the <code>createRootPane</code> method to install Escape key handling.
   * <pre>
   *  When using the JDialog window through a JOptionPane, you do not have to install the Escape key handling,
   *  as the basic look-and-feel class for the option pane (BasicOptionPaneUI) already does this for you.
   * </pre>
   *
   * @return  <code>JRootPane</code> - the rootpane with some keyboard actions registered.
   *
   */
  protected JRootPane createRootPane() {
    ActionListener actionListener = new ActionListener() {
      public void actionPerformed( ActionEvent actionEvent ) {
        setVisible( false );
      }
    };
    JRootPane rootPane = new JRootPane();
    KeyStroke stroke = KeyStroke.getKeyStroke( KeyEvent.VK_ESCAPE, 0 );
    rootPane.registerKeyboardAction( actionListener, stroke, JComponent.WHEN_IN_FOCUSED_WINDOW );
    return rootPane;
  }
}
