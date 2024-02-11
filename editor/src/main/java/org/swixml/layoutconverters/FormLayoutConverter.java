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

import java.awt.LayoutManager;
import java.util.StringTokenizer;

import org.swixml.Attribute;
import org.swixml.LayoutConverter;
import org.swixml.converters.Util;
import org.w3c.dom.Element;

import com.jgoodies.forms.layout.CellConstraints;
import com.jgoodies.forms.layout.FormLayout;

/**
 * A layout converter for <code>com.jgoodies.forms.layout.FormLayout</code>.
 * 
 * <p><b>Examples:</b></p>
 * <pre>
 * &lt;panel&gt;
 *   &lt;layout type="FormLayout"
 *       columns="p, 3dlu, p:grow"
 *       rows="p, 3dlu, p"/&gt;
 *
 *   &lt;label     constraints="1,1" text="Company"/&gt;
 *   &lt;textfield constraints="3,1"/&gt;
 *   &lt;label     constraints="1,3" text="Contact"/&gt;
 *   &lt;textfield constraints="3,3"/&gt;
 * &lt;/panel&gt;
 * </pre>
 * 
 * <pre>
 * &lt;panel&gt;
 *   &lt;layout type="FormLayout"
 *       columns="right:max(40dlu;pref), 3dlu, d:grow, 7dlu, right:pref, 3dlu, d:grow"
 *       rows="p, 3dlu, p, 9dlu, p, 3dlu, p"
 *       columnGroups="1,5; 3,7"/&gt;
 *
 *   &lt;label     constraints="1,1" text="Company"/&gt;
 *   &lt;textfield constraints="3,1,5,1"/&gt;
 *   &lt;label     constraints="1,3" text="Contact"/&gt;
 *   &lt;textfield constraints="3,3,5,1"/&gt;
 *
 *   &lt;label     constraints="1,5" text="PTI [kW]"/&gt;
 *   &lt;textfield constraints="3,5"/&gt;
 *   &lt;label     constraints="5,5" text="Power [kW]"/&gt;
 *   &lt;textfield constraints="7,5"/&gt;
 *   &lt;label     constraints="1,7" text="R [mm]"/&gt;
 *   &lt;textfield constraints="3,7"/&gt;
 *   &lt;label     constraints="5,7" text="D [mm]"/&gt;
 *   &lt;textfield constraints="7,7"/&gt;
 * &lt;/panel&gt;
 * </pre>
 *
 * @author Karl Tauber
 */
public class FormLayoutConverter implements LayoutConverter {

  /**
   * Returns "formlayout".
   */
  public String getID() {
    return "formlayout";
  }

  /**
   * Returns always <code>null</code>.
   */
  public LayoutManager convertLayoutAttribute( final Attribute attr ) {
   return null;
  }

  /**
   * <p>Creates a FormLayout instance.</p>
   * 
   * <p><b>Attributes:</b></p>
   * <ul>
   * <li><code>columns</code> (required): The column specifications as documented in JGoodies FormLayout.</li>
   * <li><code>row</code> (required): The row specifications as documented in JGoodies FormLayout.</li>
   * <li><code>columnGroups</code> (optional): The column groups, where each column
   *   in a group gets the same group wide width. Groups are separated by semicolons,
   *   column indices in a group are separated by colons. E.g. "1,5; 3,7,9" defines
   *   two groups, where first group contains columns 1 and 5; and second group
   *   contains columns 3, 7 and 9. Note that column indices are 1-based.</li>
   * <li><code>rowGroups</code> (optional): The row groups, where each row
   *   in a group gets the same group wide height. Groups are separated by semicolons,
   *   row indices in a group are separated by colons. E.g. "1,5; 3,7,9" defines
   *   two groups, where first group contains rows 1 and 5; and second group
   *   contains rows 3, 7 and 9. Note that row indices are 1-based.</li>
   * </ul>
   * 
   * <p><b>Examples for Valid XML element notations:</b></p>
   * <ul>
   * <li><code>&lt;layout type="FormLayout" columns="p, 3dlu, p" rows="p, 3dlu, p"/&gt;</code></li>
   * <li><code>&lt;layout type="FormLayout" columns="p, 3dlu, p, 3dlu, p, 3dlu, p" rows="p, 3dlu, p"
   * columnGroups="1,5; 3,7" rowGroups="1,3"/&gt;</code></li>
   * </ul>
   */
  public LayoutManager convertLayoutElement( final Element element ) {
    String encodedColumnSpecs = Attribute.getAttributeValue(element,"columns");
    String encodedRowSpecs = Attribute.getAttributeValue(element,"rows");
    int[][] columnGroupIndices = convertGroupIndices(Attribute.getAttributeValue(element,"columnGroups"));
    int[][] rowGroupIndices = convertGroupIndices(Attribute.getAttributeValue(element,"rowGroups"));

    FormLayout lm = new FormLayout( encodedColumnSpecs, encodedRowSpecs );
    if (columnGroupIndices != null)
      lm.setColumnGroups(columnGroupIndices);
    if (rowGroupIndices != null)
      lm.setRowGroups(rowGroupIndices);

    return lm;
  }

  /**
   * <p>Creates a CellConstraints instance.</p>
   * 
   * <p>Allowed syntaxes of attribute value:</p>
   * <ul>
   * <li><code>"x, y"</code></li>
   * <li><code>"x, y, w, h"</code></li>
   * <li><code>"x, y, hAlign, vAlign"</code></li>
   * <li><code>"x, y, w, h, hAlign, vAlign"</code></li>
   * </ul>
   * <p>See JGoodies FormLayout for details.</p>
   * 
   * <p><b>Examples for Valid XML attribute notations:</b></p>
   * <ul>
   * <li><code>constraints="1, 3"</code></li>
   * <li><code>constraints="1, 3, 2, 1"</code></li>
   * <li><code>constraints="1, 3, left, bottom"</code></li>
   * <li><code>constraints="1, 3, 2, 1, l, b"</code></li>
   * </ul>
   */
  public Object convertConstraintsAttribute( final Attribute attr ) {
    return new CellConstraints( attr.getValue() );
  }

  /**
   * Returns always <code>null</code>.
   */
  public Object convertConstraintsElement( final Element element ) {
    return null;
  }

  private int[][] convertGroupIndices( final String groups ) {
    if (groups == null)
      return null;
    StringTokenizer st = new StringTokenizer( groups, ";" );
    int[][] groupIndices = new int[st.countTokens()][];
    int i = 0;
    while (st.hasMoreTokens() ) {
      String group = st.nextToken();
      groupIndices[i++] = Util.ia(new StringTokenizer( group, "," ));
    }
    return groupIndices;
  }
}
