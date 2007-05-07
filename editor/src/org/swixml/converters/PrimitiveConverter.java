/*--
 $Id: PrimitiveConverter.java,v 1.1 2004/03/01 07:56:02 wolfpaulus Exp $

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

package org.swixml.converters;

import org.jdom.Attribute;
import org.jdom.DataConversionException;
import org.swixml.Converter;
import org.swixml.Localizer;
import org.swixml.Parser;

import javax.swing.*;
import javax.swing.tree.TreeSelectionModel;
import java.awt.*;
import java.util.HashMap;
import java.util.Map;

/**
 * The <code>PrimitiveConverter</code> class defines a converter that creates primitive objects (wrapper),
 * based on a provided input Class and String.
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 * @see org.swixml.ConverterLibrary

 */

public class PrimitiveConverter implements Converter, SwingConstants, KeyEvent, InputEvent {

  /** converter's return type */
  public static final Class TEMPLATE = Object.class;
  /** map contains all constant provider types */
  private static Map dictionaries = new HashMap();
  /**
   * Static Initializer, setting up the initial constant providers
   */
  static {

    PrimitiveConverter.addConstantProvider( JTabbedPane.class );
    PrimitiveConverter.addConstantProvider( JScrollPane.class );
    PrimitiveConverter.addConstantProvider( GridBagConstraints.class );
    PrimitiveConverter.addConstantProvider( FlowLayout.class );
    PrimitiveConverter.addConstantProvider( ListSelectionModel.class );
    PrimitiveConverter.addConstantProvider( TreeSelectionModel.class );
    PrimitiveConverter.addConstantProvider( WindowConstants.class );
    PrimitiveConverter.addConstantProvider( JFrame.class );

  };
  /**
   * Converts String into java primitive type
   * @param type <code>Class</code> target type
   * @param attr <code>Attribute</code> value field needs to provide convertable String
   * @return <code>Object</code> primitive wrapped into wrapper object
   */
  public static Object conv( final Class type, final Attribute attr,  final Localizer localizer ) throws Exception {
    Attribute a = (Attribute) attr.clone();
    Object obj = null;
    if ( Parser.LOCALIZED_ATTRIBUTES.contains( a.getName().toLowerCase() ))
      if (a.getAttributeType() == Attribute.CDATA_TYPE )
         a.setValue( localizer.getString( a.getValue() ));

    try {
      if (boolean.class.equals( type )) {
        obj = new Boolean( a.getBooleanValue() );
      } else if (int.class.equals( type )) {
        obj = new Integer( a.getIntValue() );
      } else if (long.class.equals( type )) {
        obj = new Long( a.getLongValue() );
      } else if (float.class.equals( type )) {
        obj = new Float( a.getFloatValue() );
      } else if (double.class.equals( type )) {
        obj = new Double( a.getDoubleValue() );
      }
    } catch (DataConversionException e) {
    } finally {
      if (obj==null) {
        try {
          String s = a.getValue();
          int k = s.indexOf( '.' ) - 1;
          Class pp = (Class) dictionaries.get( s.substring( 0, s.indexOf( '.' ) ) );
          obj = pp.getField( s.substring( k + 2 ) ).get( pp );
        } catch (Exception ex) {
          //
          //  Try to find the given value as a Constant in SwingConstants
          //
          obj = PrimitiveConverter.class.getField( a.getValue() ).get( PrimitiveConverter.class );
        }
      }
    }

    return obj;
  }

  /**
   * Converts String into java primitive type
   * @param type <code>Class</code> target type
   * @param attr <code>Attribute</code> value field needs to provide convertable String
   * @return <code>Object</code> primitive wrapped into wrapper object
   * @throws Exception
   */
  public Object convert( final Class type, final Attribute attr, final Localizer localizer ) throws Exception {
    return PrimitiveConverter.conv( type, attr, localizer );
  }

  /**
   * A <code>Converters</code> conversTo method informs about the Class type the converter
   * is returning when its <code>convert</code> method is called
   * @return <code>Class</code> - the Class the converter is returning when its convert method is called
   */
  public Class convertsTo() {
    return TEMPLATE;
  }

  /**
   * Adds a new class or interface to the dictionary of primitive providers.
   * @param type <code>Class</code> providing primitive constants / public (final) fields
   */
  public static void addConstantProvider(final Class type) {
    String shortName = type.getName().substring(type.getName().lastIndexOf( '.' ) + 1 );
    dictionaries.put( shortName, type );
  }
}
