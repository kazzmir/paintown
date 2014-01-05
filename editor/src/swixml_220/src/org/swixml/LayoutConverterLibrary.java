/*--
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

import org.swixml.layoutconverters.*;
import java.awt.*;
import java.util.*;

/**
 * <p>The <code>LayoutConverterLibrary</code> contains all available {@link LayoutConverter}s.</p>
 *
 * @author Karl Tauber
 * @see LayoutConverter
 */
public class LayoutConverterLibrary {
  private static LayoutConverterLibrary instance = new LayoutConverterLibrary();
  private Map<String, LayoutConverter> layoutConverters = new HashMap<String, LayoutConverter>();
  private Map<String, LayoutConverter> layoutIDs = new HashMap<String, LayoutConverter>();

  /**
   * The only available Ctor is private to make this a Singleton
   */
  private LayoutConverterLibrary() {
    registerLayoutConverters();
  }

  /**
   * Returns the single instacne of the LayoutConverterLibrary.
   */
  public static synchronized LayoutConverterLibrary getInstance() {
    return instance;
  }

  /**
   * Returns all registered layout converters.
   */
  public Map getLayoutConverters() {
    return layoutConverters;
  }

  /**
   * Registers <code>LayoutConverters</code> with the LayoutConverterLibrary.
   */
  private void registerLayoutConverters() {
    register(BorderLayout.class, new BorderLayoutConverter());
    register(CardLayout.class, new CardLayoutConverter());
    register(FlowLayout.class, new FlowLayoutConverter());
    register(GridBagLayout.class, new GridBagLayoutConverter());
    register(GridLayout.class, new GridLayoutConverter());

    // 3rd party layout managers
    register("com.jgoodies.forms.layout.FormLayout", new FormLayoutConverter());
  }

  /**
   * Registers a LayoutConverter with the LayoutConverterLibrary.
   * @param layoutClass Type of the layout manager the LayoutConverter creates.
   * @param layoutConverter Instance of LayoutConverter able to convert Strings
   *                        into layout managers or layout constraints.
   */
  public void register( Class layoutClass, LayoutConverter layoutConverter ) {
    register(layoutClass.getName(), layoutConverter);
  }

  /**
   * Registers a LayoutConverter with the LayoutConverterLibrary.
   * @param layoutClassName Type name of the layout manager the LayoutConverter creates.
   * @param layoutConverter Instance of LayoutConverter able to convert Strings
   *                        into layout managers or layout constraints.
   */
  public void register( String layoutClassName, LayoutConverter layoutConverter ) {
    layoutConverters.put(layoutClassName, layoutConverter);
    layoutIDs.put(layoutConverter.getID().toLowerCase(), layoutConverter);
  }

  /**
   * Returns a <code>LayoutConverter</code> instance, able to produce objects for
   * the given layout manager <code>class</code>.
   * @param layoutClass Class of the object the <code>LayoutConverter</code> needs to produce.
   * @return Instance of the LayoutConverter class.
   */
  public LayoutConverter getLayoutConverter( Class layoutClass ) {
    return layoutConverters.get( layoutClass.getName() );
  }

  /**
   * Returns a <code>LayoutConverter</code> instance, able to produce objects for
   * the given layout manager <code>id</code> (see {@link LayoutConverter#getID()}).
   * @param id Identifier of the layout manager the <code>LayoutConverter</code> needs to produce.
   * @return Instance of the LayoutConverter class.
   */
  public LayoutConverter getLayoutConverterByID( String id ) {
    return layoutIDs.get( id.toLowerCase() );
  }
}
