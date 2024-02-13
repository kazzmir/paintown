/*--
 $Id: Util.java,v 1.1 2004/03/01 07:56:03 wolfpaulus Exp $

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

import java.util.StringTokenizer;

import org.swixml.Attribute;
import org.w3c.dom.Element;

/**
 * Util. Class with static helper methods
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $

 */
public final class Util {
  /**
   * Returns the remaining tokens of a StringTokenizer in an int-Array
   * @param st <code>StringTokenizer</code>
   * @return <code>int[]</code> array containing the remaining tokens converted into int(s)
   */
  public static int[] ia( StringTokenizer st ) {
    int size = st != null ? st.countTokens() : 0;
    int[] a = new int[size];
    int i = 0;
    while (st != null && st.hasMoreTokens()) {
      try {
        a[i] = Integer.parseInt( st.nextToken().trim() );
        i++;
      } catch (NumberFormatException e) {
        // no exceptiion handling required
      }
    }
    int[] b = new int[i];
    System.arraycopy( a, 0, b, 0, i );
    return b;
  }

  /**
   * Returns the remaining tokens of a StringTokenizer in a double-Array
   * @param st <code>StringTokenizer</code>
   * @return <code>double[]</code> array containing the remaining tokens converted into double(s)
   */
  public static double[] da( StringTokenizer st ) {
    int size = st != null ? st.countTokens() : 0;
    double[] a = new double[size];
    int i = 0;
    while (st != null && st.hasMoreTokens()) {
      try {
        a[i] = Double.parseDouble( st.nextToken().trim() );
        i++;
      } catch (NumberFormatException e) {
        // no exceptiion handling required
      }
    }
    double[] b = new double[i];
    System.arraycopy( a, 0, b, 0, i );
    return b;
  }
  /**
   * Return a capitalized version of the specified property name.
   *
   * @param s <code>String</code> The property name
   * @return <code>String</code> given String with 1st letter capitalized
   */
  public static final String capitalize( final String s ) {
    String cs = null;
    if (s != null && 0 < s.length()) {
      final char[] chars = s.toCharArray();
      chars[0] = Character.toUpperCase( chars[0] );
      cs = new String( chars );
    }
    return cs;
  }

  /**
   * Returns the integer value of the given XML attribute; or the default value.
   */
  public static final int getInteger( final Element element, final String attr, int def ) {
    String value = Attribute.getAttributeValue(element,attr);
    if (value == null)
      return def;

    try {
      return Integer.parseInt( value.trim() );
    } catch (NumberFormatException e) {
      // no exceptiion handling required
      return def;
    }
  }
}
