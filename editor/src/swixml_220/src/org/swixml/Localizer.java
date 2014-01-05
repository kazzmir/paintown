/*--
 $Id: Localizer.java,v 1.2 2004/08/20 05:59:57 wolfpaulus Exp $

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

import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;

/**
 * The <code>Localizer</code> class provides consumers with a simple localization tools: <code>getString(key)</code>.
 * Locale and ResourceBundle need to be set to use it.
 * Since some setters accept comma separated lists of Strings, e.g. a TabbedPane's setTitles methods, the Localizer will
 * try to split a given key by <i>commas</i> if the key doesn't resolve, i.e. a <code>MissingResourceException</code> is
 * thrown. <br />
 * For example, if the resource bundle contains strings for the following single keys: <br />
 * a = Alpha <br />
 * b = Bravo <br />
 * c = Charlie<br />
 * then calling getString(&quot;a,b,c&quot;) will result in a String containing the comma sepearted values, like
 * &quot;Alpha,Brave,Charlie&quot; <br />
 * Look at the provided testcase for more details.
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.2 $
 * @see XTabbedPane#setTitles(String)
 */
public class Localizer {
  private static String SEPARATOR = ",";
  private Locale locale = Locale.getDefault();
  private String bundleName;
  private ResourceBundle bundle;
  private ClassLoader cl = Localizer.class.getClassLoader();

  /**
   * Sets the regular expression used to split a key, that could not be found in the resource bundle.
   *
   * @param regExp <code>String</code>
   * @see String#split
   * @see #getString
   */
  public static void setSeparator(String regExp) {
    Localizer.SEPARATOR = regExp;
  }

  /**
   * Returns the localized String baseed on the given key.
   * If the key cannot be found, the key is returned insstead.
   *
   * @param key <code>String</code>
   * @return <code>String</code> - localized String , or key , if no lacalization is found.
   */
  public String getString(final String key) {
    if (!isUsable()) return key;
    String s = "";
    try {
      s = bundle.getString(key);
    } catch (MissingResourceException e) {
      String[] keys = key.split(Localizer.SEPARATOR);
      if (2 <= keys.length) {
        for (int i = 0; i < keys.length; i++) {
          s += (i == 0) ? getString(keys[i]) : "," + getString(keys[i]);
        }
      } else s = key;
    } catch (Exception e) {
      s = key;   // key not found, return key
    }
    return s;
  }

  /**
   * Sets this Localizer's locale.
   *
   * @param locale <code>Locale</code>
   */
  public void setLocale(Locale locale) {
    if (locale == null) {
      this.locale = null;
      this.bundle = null;
      this.bundleName = null;
    } else if (this.locale != locale) {
      this.locale = locale;
      setResourceBundle(bundleName);
    }
  }

  /**
   * Sets this Localizer's ResourceBundle.
   *
   * @param bundleName <code>String</code>ResourceBundle file / class name
   * @throws java.util.MissingResourceException
   *          - if no resource bundle for the specified base name can be found
   */
  public void setResourceBundle(String bundleName) throws MissingResourceException {
    this.bundleName = bundleName;
    if (locale == null) {
      locale = Locale.getDefault();
    }
    if (bundleName != null) {
      bundle = ResourceBundle.getBundle(bundleName, locale, cl);
    } else {
      bundle = null;
    }
  }

  /**
   * Informs about the usablility of this Localizer.
   *
   * @return <code>boolean</code> - true if Localizer is setup with Locale and ResourceBundle.
   */
  public boolean isUsable() {
    return (locale != null && bundle != null);
  }

  /**
   * @return <code>ClassLoader</code> returns the classloader attribute, which has probably been set by the SwingEngine
   */
  public ClassLoader getClassLoader() {
    return cl;
  }

  /**
   * Sets the ClassLoader attribute.
   * The Localizer does not use the provided classloader directly but return it when asked for.
   *
   * @param cl <code>ClassLoader</code> - custom classloader
   */
  void setClassLoader(ClassLoader cl) {
    this.cl = cl;
  }
}
