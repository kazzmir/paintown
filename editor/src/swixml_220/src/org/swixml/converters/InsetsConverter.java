/*--
 $Id: InsetsConverter.java,v 1.1 2004/03/01 07:56:00 wolfpaulus Exp $

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

import java.awt.*;
import java.util.StringTokenizer;

/**
 * The <code>InsetsConverter</code> class defines a converter that creates Insets objects based on a provided String.
 *
 * <h3>Examples for Valid XML attribute notations:</h3>
 * <pre>
 * <ul>
 * <li>border="MatteBorder(4,4,4,4,red)"</li>
 * <li>insets="2,2,2,2"</li>
 * </ul>
 * </pre>
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 *
 * @see java.awt.Insets
 * @see org.swixml.ConverterLibrary

 */
public class InsetsConverter implements Converter {

  /** converter's return type */
  public static final Class TEMPLATE = Insets.class;


  /**
   * Converts a Strings into an Insets object
   *
   * @param type <code>Class</code> not used
   * @param attr <code>Attribute</code> value needs to provide String containing comma sep. integers
   * @return <code>Object</code> runtime type is subclass of <code>Insets</code>
   */
  public Object convert( final Class type, final Attribute attr, Localizer localizer ) {
    Insets insets = null;
    if (attr != null) {
      StringTokenizer st = new StringTokenizer( attr.getValue(), "(,)" );
      if (5 == st.countTokens()) { // assume "insets(...)"
        st.nextToken().trim();
      }
      int[] param = Util.ia( st );
      if (4 <= param.length) {
        insets = new Insets( param[0], param[1], param[2], param[3] );
      }
    }
    return insets;
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
