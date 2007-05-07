/*--
 $Id: LayoutConverter.java,v 1.1 2004/03/01 07:56:01 wolfpaulus Exp $

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
import org.swixml.Converter;
import org.swixml.Localizer;
import org.swixml.SwingEngine;

import java.awt.*;
import java.lang.reflect.Field;
import java.util.StringTokenizer;

/**
 * The <code>LayoutConverter</code> class defines a converter that creates LayoutManager objects
 * based on a provided String.
 * <h3>Examples for Valid XML attribute notations:</h3>
 * <pre>
 * <p><b>FlowLayout</b><br>can be created with all 3 construtors:<br>
 * <ul>
 * <li>FlowLayout()</li>
 * <li>FlowLayout(int align)</li>
 * <li>FlowLayout(int align, int hgap, int vgap)</li>
 * </ul>
 *
 * <ul>
 * <li>BorderLayout()</li>
 * <li>BorderLayout(int hgap, int vgap)</li>
 * </ul>
 *
 * <ul>
 * <li>GridBagLayout()</li>
 * <li>GridBagLayout(rowWeights(0,0,1.0,0))</li>
 * <li>GridBagLayout(colWeights(0.5, 0.5, 1.0, 99.9))</li>
 * <li>GridBagLayout(columnWidths(5, 5, 10, 33))</li>
 * <li>GridBagLayout(rowHeights(5, 5, 10, 33))</li>
 * </ul>
 *
 * </pre>
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 *
 * @version $Revision: 1.1 $
 *
 * @see java.awt.LayoutManager
 * @see org.swixml.ConverterLibrary

 */
public class LayoutConverter implements Converter {
  /** converter's return type */
  public static final Class TEMPLATE = LayoutManager.class;

  public static final String FLOW_LAYOUT = "flowlayout";
  public static final String BORDER_LAYOUT = "borderlayout";
  public static final String GRID_LAYOUT = "gridlayout";
  public static final String GRIDBAG_LAYOUT = "gridbaglayout";
  public static final String CARD_LAYOUT = "cardlayout";

  /**
   * Convert the specified input object into an output object of the
   * specified type.
   * <pre>
   * <p><b>FlowLayout</b><br>can be created with all 3 construtors:<br>
   * <ul>
   * <li>FlowLayout()</li>
   * <li>FlowLayout(int align)</li>
   * <li>FlowLayout(int align, int hgap, int vgap)</li>
   * </ul>
   *
   * <ul>
   * <li>BorderLayout()</li>
   * <li>BorderLayout(int hgap, int vgap)</li>
   * </ul>
   *
   * <ul>
   * <li>BoxLayout: see Box tag</li>
   * </ul>
   *
   * <ul>
   * <li>GridLayout()</li>
   * <li>GridLayout(int rows, int cols)</li>
   * <li>GridLayout(int rows, int cols, int hgap, int vgap) </li>
   * </ul>
   *
   * <ul>
   * <li>GridBagLayout()</li>
   * <li>GridBagLayout(rowWeights(0,0,1.0,0))</li>
   * <li>GridBagLayout(colWeights(0.5, 0.5, 1.0, 99.9))</li>
   * <li>GridBagLayout(columnWidths(5, 5, 10, 33))</li>
   * <li>GridBagLayout(rowHeights(5, 5, 10, 33))</li>
   * </ul>
   *
   * <ul>
   * <li>CardLayout() - Creates a new card layout with gaps of size zero.</li>
   * <li>CardLayout(int hgap, int vgap) - Creates a new card layout with the specified horizontal and vertical gaps.</li>
   * </ul>
   * Here is how to access the card layout manager of a component installed by SwixML
   * (CardLayout)((Container)swingEngine.find("id_of_my_CLed_comp")).getLayout()
   *
   * </pre>
   * @param type <code>Class</code> not used
   * @param attr <code>Attribute</code> value is needed for conversion
   * @return <code>Object</code>
   */
  public Object convert( final Class type, final Attribute attr, final Localizer localizer ) {
    LayoutManager lm = null;
    StringTokenizer st = new StringTokenizer( attr.getValue(), "(,)" );
    String s = st.nextToken().trim();  // here, s should contain the LayoutManager's name
    if (s != null) {
      s = s.toLowerCase();
      if (s.equals( LayoutConverter.FLOW_LAYOUT )) {
        //
        //  FLOW_LAYOUT
        //
        try {
          if (st.hasMoreTokens()) {
            //
            //  First FlowLayout parameter might be a pre-defined constant's name
            //
            Object o = PrimitiveConverter.conv( null, new Attribute( "NA", st.nextToken() ), localizer );
            int[] para = Util.ia( st );
            //
            //  Remaining paramters should be integer values
            //
            if (para.length < 2)
              lm = new FlowLayout( Integer.valueOf( o.toString() ).intValue() );
            else
              lm = new FlowLayout( Integer.valueOf( o.toString() ).intValue(), para[ 0 ], para[ 1 ] );
          }
        } catch (Exception e) {
        }
        if (lm == null) {
          lm = new FlowLayout();
        }
      } else if (s.equals( LayoutConverter.BORDER_LAYOUT )) {
        //
        //  BORDER_LAYOUT
        //
        int[] para = Util.ia( st );
        if (para.length < 2)
          lm = new BorderLayout();
        else
          lm = new BorderLayout( para[ 0 ], para[ 1 ] );
      } else if (s.equals( LayoutConverter.GRID_LAYOUT )) {
        //
        //  GRID_LAYOUT
        //
        int[] para = Util.ia( st );
        if (4 <= para.length)
          lm = new GridLayout( para[ 0 ], para[ 1 ], para[ 2 ], para[ 3 ] );
        else if (2 <= para.length)
          lm = new GridLayout( para[ 0 ], para[ 1 ] );
        else
          lm = new GridLayout();
      } else if (s.equals( LayoutConverter.CARD_LAYOUT )) {
        //
        //  CARD_LAYOUT
        //
        //
        int[] para = Util.ia( st );
        if (para.length < 2)
          lm = new CardLayout();
        else
          lm = new CardLayout( para[ 0 ], para[ 1 ] );
      } else if (s.equals( LayoutConverter.GRIDBAG_LAYOUT )) {
        //
        //  Gridbag Layouts have some public arrays, accept one but only one.
        //    public double[]  rowWeights
        //    public double[]  colWeights
        //
        lm = new GridBagLayout();

        if (st.hasMoreTokens()) {
          try {
            String fieldname = st.nextToken();
            Field field = GridBagLayout.class.getField( fieldname );
            if (field != null) {
              Class fieldtype = field.getType();

              if (int[].class.equals( fieldtype )) {
                field.set( lm, Util.ia( st ) );
              } else if (double[].class.equals( fieldtype )) {
                field.set( lm, Util.da( st ) );
              }

            }
          } catch (NoSuchFieldException e) {
            if (SwingEngine.DEBUG_MODE)
              System.err.println( e.getMessage() );
          } catch (SecurityException e) {
            if (SwingEngine.DEBUG_MODE)
              System.err.println( e.getMessage() );
          } catch (IllegalArgumentException e) {
            if (SwingEngine.DEBUG_MODE)
              System.err.println( e.getMessage() );
          } catch (IllegalAccessException e) {
            if (SwingEngine.DEBUG_MODE)
              System.err.println( e.getMessage() );
          }
        }
      }
    }
    return lm;
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
