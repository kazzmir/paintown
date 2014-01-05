/*--
 $Id: KeyStrokeConverter.java,v 1.1 2004/03/01 07:56:01 wolfpaulus Exp $

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

package org.swixml.converters;

import org.swixml.Attribute;
import org.swixml.Converter;
import org.swixml.Localizer;
import org.swixml.Parser;

import javax.swing.*;

/**
 * A Converter that turns a Strings in the form of a filename into an KeyStroke objects.
 * <pre>
 *   Valild syntax includes:
 *   <ul>
 *   <li>accelerator="F7"
 *   <li>accelerator="control N"
 *   <li>accelerator="alt A"
 *   </ul>
 * </pre>
 * <hr>
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @author <a href="mailto:Eric.LAFARGUE@cec.eu.int">Eric Lafargue</a>
 * @version $Revision: 1.1 $
 * @see javax.swing.KeyStroke#getKeyStroke(java.lang.String)
 * @see org.swixml.ConverterLibrary
 */
public class KeyStrokeConverter implements Converter {

  /** converter's return type */
  public static final Class TEMPLATE = KeyStroke.class;

  /**
   * Converts a String into an KeyStroke through a Resource lookup
   * @param type <code>Class</code> not used
   * @param attr <code>Attribute</code> attribute provides the value to be converted
   * @param localizer <code>Localizer</code> allow the use of resource lookups
   * @return <code>Object</code> - a <code>KeyStroke</code>
   */
  public Object convert( final Class type, final Attribute attr, Localizer localizer ) {
    return KeyStrokeConverter.conv( type, attr, localizer );
  }

  /**
   * Converts a String into an KeyStroke.
   *
   *  Parses a string and returns a <code>KeyStroke</code>.
   * The string must have the following syntax:
   * <pre>
   *    &lt;modifiers&gt;* (&lt;typedID&gt; | &lt;pressedReleasedID&gt;)
   *
   *    modifiers := shift | control | ctrl | meta | alt | button1 | button2 | button3
   *    typedID := typed &lt;typedKey&gt;
   *    typedKey := string of length 1 giving Unicode character.
   *    pressedReleasedID := (pressed | released) key
   *    key := KeyEvent key code name, i.e. the name following "VK_".
   * </pre>
   * If typed, pressed or released is not specified, pressed is assumed. Here
   * are some examples:
   * <pre>
   *     "INSERT" => getKeyStroke(KeyEvent.VK_INSERT, 0);
   *     "control DELETE" => getKeyStroke(KeyEvent.VK_DELETE, InputEvent.CTRL_MASK);
   *     "alt shift X" => getKeyStroke(KeyEvent.VK_X, InputEvent.ALT_MASK | InputEvent.SHIFT_MASK);
   *     "alt shift released X" => getKeyStroke(KeyEvent.VK_X, InputEvent.ALT_MASK | InputEvent.SHIFT_MASK, true);
   *     "typed a" => getKeyStroke('a');
   * </pre>
   *
   * In order to maintain backward-compatibility, specifying a null String,
   * or a String which is formatted incorrectly, returns null.
   *
   * @param type <code>Class</code> not used
   * @param attr <code>Attribute</code> attribute provides the value to be converted
   * @param localizer <code>Localizer</code> allow the use of resource lookups
   * @return <code>Object</code> - a <code>KeyStroke</code> object for that String,
   *          or null if the specified String is null, or is formatted incorrectly
   */
  public static Object conv( final Class type, final Attribute attr, Localizer localizer ) {
    KeyStroke keyStroke = null;
    if (attr != null) {
      if (Parser.LOCALIZED_ATTRIBUTES.contains( attr.getName().toLowerCase() )) {
          attr.setValue( localizer.getString( attr.getValue() ) );
      }
      keyStroke = KeyStroke.getKeyStroke( attr.getValue() );
    }
    return keyStroke;
  }

  /**
   * A <code>Converters</code> conversTo method informs about the Class type the converter
   * is returning when its <code>convert</code> method is called
   * @return <code>Class</code> - the Class the converter is returning when its convert method is called
   */
  public Class convertsTo() {
    return TEMPLATE;
  }
}
