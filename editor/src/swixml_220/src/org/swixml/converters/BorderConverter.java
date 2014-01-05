/*--
 $Id: BorderConverter.java,v 1.1 2004/03/01 07:55:58 wolfpaulus Exp $

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
import org.swixml.ConverterLibrary;
import org.swixml.Localizer;
import org.swixml.SwingEngine;

import javax.swing.*;
import javax.swing.border.Border;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

/**
 * The <code>BorderConverter</code> class defines a converter that creates Border objects based on a provided String.
 * The BorderConverter internally uses the <code>javax.swing.BorderFactory</code> and its static <i>create</i>.. methods
 * to instantiate different kinds of borders, based on the given String.<br>
 * Additional parameters to create a border need to be comma separated and enclosed in parentheses.<br><br>
 * Parameter types get converted through the <code>ConverterLibrary</code>. Therefore, only parameter classes are supported that
 * have registered  converters in the ConverLibrary.
 * <h3>Examples for Valid XML attribute notations:</h3>
 * <pre>
 * <ul>
 * <li>border="MatteBorder(4,4,4,4,red)"</li>
 * <li>border="EmptyBorder(5,5,5,5)"</li>
 * <li>border="TitledBorder(My Title)"</li>
 * <li>border="RaisedBevelBorder"</li>
 * <li>border="TitledBorder(TitledBorder, myTitle, TitledBorder.CENTER, TitledBorder.BELOW_BOTTOM, VERDANA-BOLD-18, blue)"</li>
 * <li>border="TitledBorder(LineBorder(red,2), myTitle, TitledBorder.CENTER, TitledBorder.ABOVE_TOP)"</li>
 * <li>border="CompoundBorder(LineBorder(blue,5), CompoundBorder(EmptyBorder(2,2,2,2), RaisedBevelBorder))"</li>
 * </ul>
 * </pre>
 *
 * @author <a href="mailto:wolf@wolfpaulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 * @see javax.swing.BorderFactory
 * @see javax.swing.border.AbstractBorder
 * @see org.swixml.ConverterLibrary
 */
public class BorderConverter implements Converter {
  /**
   * converter's return type
   */
  public static final Class TEMPLATE = Border.class;

  /**
   * all methods the BorderFactory provides
   */
  private static final Method[] METHODS = BorderFactory.class.getMethods();

  /**
   * Returns a <code>javax.swing Border</code>
   *
   * @param type <code>Class</code>  not used
   * @param attr <code>Attribute</code> value needs to provide Border type name and optional parameter
   * @return <code>Object</code> runtime type is subclass of <code>AbstractBorder</code>
   */
  public Object convert(final Class type, final Attribute attr, Localizer localizer) {
    Border border = null;
    List<String> params = parse(attr.getValue()); // border type + parameters
    int n = params.size() - 1; // number of parameter to create a border
    String borderType = params.remove(0).trim();
    Method method = null;
    ConverterLibrary cvtlib = ConverterLibrary.getInstance();
    //
    // Special case for single parameter construction, give priority to String Type
    //
    if (n == 0) {
      try {
        method = BorderFactory.class.getMethod("create" + borderType);

      } catch (NoSuchMethodException e) {
        // intent. empty
      }
      if (method == null) { // try with empty string
        n = 1;
        params.add(" ");
      }
    }
    if (n == 1) {
      try {
        method = BorderFactory.class.getMethod("create" + borderType, new Class[]{String.class});
      } catch (NoSuchMethodException e) {
        //  no need to do anything here.
      }
    }
    for (int i = 0; method == null && i < METHODS.length; i++) {
      if (METHODS[i].getParameterTypes().length == n && METHODS[i].getName().endsWith(borderType)) {
        method = METHODS[i];

        for (int j = 0; j < method.getParameterTypes().length; j++) {
          if (String.class.equals(method.getParameterTypes()[j])) {
            continue;
          }
          if (null == cvtlib.getConverter(method.getParameterTypes()[j])) {
            method = null;
            break;
          }
        }
      }
    }
    try {
      Object[] args = new Object[n];
      for (int i = 0; i < n; i++) { // fill argument array
        Converter converter = cvtlib.getConverter(method.getParameterTypes()[i]);
        Attribute attrib =
          new Attribute(String.class.equals(converter.convertsTo()) ? "title" : "NA", params.remove(0).trim());
        if (converter != null) {
          args[i] = converter.convert(method.getParameterTypes()[i], attrib, localizer);
        } else {
          args[i] = attrib.getValue();
        }
      }
      border = (Border) method.invoke(null, args);
    } catch (Exception e) {
      if (SwingEngine.DEBUG_MODE)
        System.err.println("Couldn't create border, " + attr.getValue() + "\n" + e.getMessage());
    }
    return border;
  }

  /**
   * A <code>Converters</code> conversTo method informs about the Class type the converter
   * is returning when its <code>convert</code> method is called
   *
   * @return <code>Class</code> - the Class the converter is returning when its convert method is called
   */
  public Class convertsTo() {
    return TEMPLATE;
  }
  
  /**
   * Parse a border attribute value into the border type and parameters, where the parameters may specify
   * additional <code>Border</code> constructors. For example, the following string <br>
   * "CompoundBorder(LineBorder(blue,5), CompoundBorder(EmptyBorder(2,2,2,2), RaisedBevelBorder))" <br>
   * would be parsed into three strings: <br>
   * CompoundBorder <br>
   * LineBorder(blue,5) <br>
   * CompoundBorder(EmptyBorder(2,2,2,2), RaisedBevelBorder) <br>
   * 
   * @param input <code>String</code> containing the value to parse
   * @return <code>List&lt;String&gt;</code> containing the values parsed
   */
  private List<String> parse(String input) {
    ArrayList<String> tokens = new ArrayList<String>();
    int index = input.indexOf('(');
    if (index < 0) {
      tokens.add(input);
    }
    else {
      tokens.add(input.substring(0, index));
      while (++index < input.length()) {
        int depth = 0;
        int beginIndex = index;
        while ((depth >= 0) && (++index < input.length())) {
          if ((input.charAt(index) == ',') && (depth == 0)) {
            break;
          }
          if (input.charAt(index) == '(') {
            ++depth;
          }
          else if (input.charAt(index) == ')') {
            --depth;
          }
        }
        if ( index < input.length()) {
          tokens.add(input.substring(beginIndex, index));
        }
      }
    }
    return tokens;
  }
  
}
