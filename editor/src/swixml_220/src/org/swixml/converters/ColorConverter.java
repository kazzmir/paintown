/*--
 $Id: ColorConverter.java,v 1.1 2004/03/01 07:55:58 wolfpaulus Exp $

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
import org.swixml.SwingEngine;

import java.awt.*;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.StringTokenizer;

/**
 * The ColorConverter class defines a Converter that turns the Strings into a Color object
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 *
 * @see java.awt.Color

 *
 * <h3>Examples for Valid XML attribute notations:</h3>
 * <pre><b>The following example show valid xml attributes to create Color objects:</b><br>
 * <ul>
 * <li>background="3399CC"</li>
 * <li>background="red"</li>
 * <li>foreground="991144"</li>
 * </ul>
 * </pre>
 */
public class ColorConverter implements Converter {
  /** converter's return type */
  public static final Class TEMPLATE = Color.class;

  /**
    * Returns a <code>java.awt.Color</code> runtime object
    * @param type <code>Class</code> not used
    * @param attr <code>Attribute</code> value needs to provide a String
    * @return runtime type is subclass of <code>java.awt.Color</code>
    */
  public Object convert( final Class type, final Attribute attr, Localizer localizer ) {
    return ColorConverter.conv(type,attr);
  }
  /**
   * Returns a <code>java.awt.Color</code> runtime object
   * @param type <code>Class</code> not used
   * @param attr <code>Attribute</code> value needs to provide a String
   * @return runtime type is subclass of <code>java.awt.Color</code>
   */
  public static Object conv( final Class type, final Attribute attr ) {
    if (attr != null) {
      try {
        Field field = Color.class.getField( attr.getValue() );
        if (Color.class.equals( field.getType() ) && Modifier.isStatic( field.getModifiers() ))
          return field.get( Color.class );
      } catch (NoSuchFieldException e) {
      } catch (SecurityException e) {
      } catch (IllegalAccessException e) {
      }
      StringTokenizer st = new StringTokenizer( attr.getValue(), "," );
      if (1 == st.countTokens()) {
        try {
          return new Color( Integer.parseInt( st.nextToken().trim(), 16 ) );
        } catch (NumberFormatException e) {
           if (SwingEngine.DEBUG_MODE) System.err.println( e );
          return null;
        }
      }
      int[] para = Util.ia( st );
      if (4 <= para.length)
        return new Color( para[0], para[1], para[2], para[3] );
      if (3 <= para.length)
        return new Color( para[0], para[1], para[2] );
      if (1 <= para.length)
        return new Color( para[0] );
    }
    return null;
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
