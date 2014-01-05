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

import java.awt.LayoutManager;

import org.w3c.dom.Element;

/**
 * <p>The <code>LayoutConverter</code> interface defines a layout converter that
 * creates LayoutManager objects and constraints objects based on a provided
 * XML attributes or elements.</p>
 * 
 * @author Karl Tauber
 * @see LayoutConverterLibrary
 */
public interface LayoutConverter {

  /**
   * Returns the unique identifier of the layout converter. E.g. "flowlayout".
   * This identifier is used in "layout" attributes and elements to specify the
   * layout manager.
   */
  public String getID();

  /**
   * <p>Converts the given "layout" attribute to a <code>LayoutManager</code>
   * instance. The attribute value always starts with the layout identifier
   * (see {@link #getID()}) followed by optional parameters.</p>
   * 
   * <p>If the layout converter does not use the "layout" attribute, this method
   * should return <code>null</code>, but then {@link #convertLayoutElement}
   * must return a <code>LayoutManager</code> instance.</p>
   * 
   * <p><b>XML notation:</b></p>
   * <pre>
   * &lt;panel layout="mylayout"/&gt;
   * </pre>
   */
  public LayoutManager convertLayoutAttribute( final Attribute attr );

  /**
   * <p>Converts the given "layout" element to a <code>LayoutManager</code>
   * instance. The element may have any layout manager specific attributes
   * or child elements. The "type" attribute is used to choose the layout converter.</p>
   * 
   * <p>If the layout converter does not use the "layout" element, this method
   * should return <code>null</code>, but then {@link #convertLayoutAttribute}
   * must return a <code>LayoutManager</code> instance.</p>
   * 
   * <p><b>XML notation:</b></p>
   * <pre>
   * &lt;panel&gt;
   *   &lt;layout type="mylayout" attr1="value1"/&gt;
   * &lt;/panel&gt;
   * </pre>
   */
  public LayoutManager convertLayoutElement( final Element element );

  /**
   * <p>Converts the given "constraints" attribute to a layout manager specific
   * constraints instance.</p>
   * 
   * <p>If the layout converter does not use the "constraints" attribute, this method
   * should return <code>null</code>.</p>
   * 
   * <p><b>XML notation:</b></p>
   * <pre>
   * &lt;label constraints="value" /&gt;
   * </pre>
   */
  public Object convertConstraintsAttribute( final Attribute attr );

  /**
   * <p>Converts the given "constraints" element to a layout manager specific
   * constraints instance. The element may have any layout manager specific attributes
   * or child elements.</p>
   * 
   * <p>If the layout converter does not use the "constraints" element, this method
   * should return <code>null</code>.</p>
   * 
   * <p><b>XML notation:</b></p>
   * <pre>
   * &lt;label&gt;
   *   &lt;constraints attr1="value1" /&gt;
   * &lt;/label&gt;
   * </pre>
   */
  public Object convertConstraintsElement( final Element element );
}
