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

package org.swixml.layoutconverters;

import java.awt.GridLayout;
import java.awt.LayoutManager;
import java.util.StringTokenizer;

import org.swixml.Attribute;
import org.swixml.LayoutConverter;
import org.swixml.converters.Util;
import org.w3c.dom.Element;

/**
 * A layout converter for <code>java.awt.GridLayout</code>.
 * 
 * <p><b>Examples:</b></p>
 * <pre>
 * &lt;panel layout="GridLayout"&gt;
 *   &lt;panel ... /&gt;
 *   &lt;panel ... /&gt;
 * &lt;/panel&gt;
 * </pre>
 *
 * <pre>
 * &lt;panel layout="GridLayout(2,4)"&gt;
 *   ...
 * &lt;/panel&gt;
 * </pre>
 *
 * <pre>
 * &lt;panel layout="GridLayout(2,4,10,20)"&gt;
 *   ...
 * &lt;/panel&gt;
 * </pre>
 *
 * <pre>
 * &lt;panel&gt;
 *   &lt;layout type="GridLayout" rows="2" columns="4"/&gt;
 *   ...
 * &lt;/panel&gt;
 * </pre>
 *
 * <pre>
 * &lt;panel&gt;
 *   &lt;layout type="GridLayout" rows="2" columns="4" hgap="10" vgap="20"/&gt;
 *   ...
 * &lt;/panel&gt;
 * </pre>
 *
 * @author Karl Tauber
 * @author <a href="mailto:wolf@wolfpaulus.com">Wolf Paulus</a>
 */
public class GridLayoutConverter implements LayoutConverter {

  /**
   * Returns "gridlayout".
   */
  public String getID() {
    return "gridlayout";
  }

  /**
   * <p>Creates a GridLayout instance.</p>
   * 
   * <p><b>Examples for Valid XML attribute notations:</b></p>
   * <ul>
   * <li><code>layout="GridLayout"</code></li>
   * <li><code>layout="GridLayout(int rows, int cols)"</code></li>
   * <li><code>layout="GridLayout(int rows, int cols, int hgap, int vgap)"</code></li>
   * </ul>
   */
  public LayoutManager convertLayoutAttribute( final Attribute attr ) {
    StringTokenizer st = new StringTokenizer( attr.getValue(), "(,)" );
    st.nextToken(); // skip layout type

    int[] para = Util.ia( st );
    if (4 <= para.length)
      return new GridLayout( para[ 0 ], para[ 1 ], para[ 2 ], para[ 3 ] );
    else if (2 <= para.length)
      return new GridLayout( para[ 0 ], para[ 1 ] );
    else
      return new GridLayout();
  }

  /**
   * <p>Creates a GridLayout instance.</p>
   * 
   * <p><b>Attributes:</b></p>
   * <ul>
   * <li><code>rows</code> (optional): The number of rows.</li>
   * <li><code>columns</code> (optional): The number of columns.</li>
   * <li><code>hgap</code> (optional): The horizontal gap.</li>
   * <li><code>vgap</code> (optional): The vertical gap.</li>
   * </ul>
   * 
   * <p><b>Examples for Valid XML element notations:</b></p>
   * <ul>
   * <li><code>&lt;layout type="GridLayout"/&gt;</code></li>
   * <li><code>&lt;layout type="GridLayout" rows="4" columns="5"/&gt;</code></li>
   * <li><code>&lt;layout type="GridLayout" rows="2" columns="4" hgap="10" vgap="20"/&gt;</code></li>
   * </ul>
   */
  public LayoutManager convertLayoutElement( final Element element ) {
    int rows = Util.getInteger(element, "rows", 1);
    int cols = Util.getInteger(element, "columns", 0);
    int hgap = Util.getInteger(element, "hgap", 0);
    int vgap = Util.getInteger(element, "vgap", 0);
    return new GridLayout(rows, cols, hgap, vgap);
  }

  /**
   * Returns always <code>null</code>.
   */
  public Object convertConstraintsAttribute( final Attribute attr ) {
    return null;
  }

  /**
   * Returns always <code>null</code>.
   */
  public Object convertConstraintsElement( final Element element ) {
    return null;
  }
}
