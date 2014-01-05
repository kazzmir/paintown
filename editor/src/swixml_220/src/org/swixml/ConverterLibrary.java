/*--
 $Id: ConverterLibrary.java,v 1.1 2004/03/01 07:56:05 wolfpaulus Exp $

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

import org.swixml.converters.*;

import javax.swing.*;
import javax.swing.border.Border;
import java.awt.*;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Locale;
import java.util.Map;

/**
 * <p>The <code>ConverterLibrary</code> contains all available Coverters, converting Strings.</p>
 * <p>General purpose data type converters are able to convert Strings into objects that are
 * usually as parameters when setters are called on javax.swing objects.
 * <pre>
 * Available Converter include converters able to produce
 * <ul>
 * <li>Primitives</li>
 * <li>Dimension</li>
 * <li>Color</li>
 * <li>Border</li>
 * <li>etc.</li>
 * </ul>
 * </pre>
 * <pre>
 * Example String inputs could look like this:
 * <ul>
 * <li>MatteBorder(4,4,4,4,red)</li>
 * <li>FFCCEE</li>
 * <li>BorderLayout.CENTER</li>
 * <li>2,2,2,2</li>
 * </ul>
 * </pre>
 * Date: Dec 16, 2002
 *
 * @author <a href="mailto:wolf@wolfpaulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 * @see org.swixml.Converter
 */
public class ConverterLibrary {
  private static ConverterLibrary instance = new ConverterLibrary();
  private Map<Class, Converter> converters = new HashMap<Class, Converter>();

  /**
   * The only available Ctor is private to make this a Singleton
   */
  private ConverterLibrary() {
    registerConverters();
  }

  /**
   * @return <code>ConverterLibrary</code> the single instacne of the ConverterLibrary.
   */
  public static synchronized ConverterLibrary getInstance() {
    return instance;
  }

  /**
   * @return <code>Map</code> - all registered converters.
   *         <pre>Use a class to get to the converters</pre>
   */
  public Map getConverters() {
    return converters;
  }

  /**
   * Registers <code>Converters</code> with the ConverterLibrary.
   */
  private void registerConverters() {
    register(Action.class, new ActionConverter());
    register(Border.class, new BorderConverter());
    register(Color.class, new ColorConverter());
    register(Component.class, new ComponentConverter());
    register(Dimension.class, new DimensionConverter());
    register(Font.class, new FontConverter());
    register(Image.class, new ImageConverter());
    register(Icon.class, new ImageIconConverter());
    register(ImageIcon.class, new ImageIconConverter());
    register(Insets.class, new InsetsConverter());
    register(KeyStroke.class, new KeyStrokeConverter());
    register(Locale.class, new LocaleConverter());
    register(Point.class, new PointConverter());
    register(Rectangle.class, new RectangleConverter());
    register(String.class, new StringConverter());
    //
    //  Register the PrimitiveConverter class for java primitive types
    //
    register(boolean.class, new PrimitiveConverter());
    register(int.class, new PrimitiveConverter());
    register(long.class, new PrimitiveConverter());
    register(float.class, new PrimitiveConverter());
    register(double.class, new PrimitiveConverter());
  }

  /**
   * Registers a Converter with the ConverterLibrary
   *
   * @param converter <code>Converter</code> Instance of Converter able to convert Strings into objects of the given type
   */
  public void register(Converter converter) {
    converters.put(converter.convertsTo(), converter);
  }

  /**
   * Registers a Converter with the ConverterLibrary
   *
   * @param template  <code>Class</code> type of the objects the Converter creates
   * @param converter <code>Converter</code> Instance of Converter able to convert Strings into objects of the given type
   */
  public void register(Class template, Converter converter) {
    converters.put(template, converter);
  }

  /**
   * Indicates if a the ConverterLibary has a Converter producing instances of the given Class.
   *
   * @param template <code>Class</code>
   * @return <code>boolean</code> true, if the ConverterLibrary has a Converter to produce an instances of the gioven class.
   */
  public boolean hasConverter(Class template) {
    boolean found = converters.keySet().contains(template);
    Iterator it = converters.values().iterator();
    while (!found && it != null && it.hasNext()) {
      found = template.isAssignableFrom(((Converter) it.next()).convertsTo());
    }
    return found;
  }

  /**
   * Returns a <code>Converter</code> instance, able to produce objects of the given <code>class</code>
   *
   * @param template <code>Class</code> Class of the object the <code>Converter</code> needs to produce.
   * @return <code>Converter</code> - instance of the given Converter class.
   */
  public Converter getConverter(Class template) {
    return converters.get(template);
  }
}
