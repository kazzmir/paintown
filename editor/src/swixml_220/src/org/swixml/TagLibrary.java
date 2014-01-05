/*--
 $Id: TagLibrary.java,v 1.1 2004/03/01 07:56:07 wolfpaulus Exp $

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

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * A skeletal impementation of a TagLibrary<br>
 * A TagLibrary has a collection of Factories.
 * Every Tag is mapped to a Factory which is used to build the java object during document parsing.
 * Date: Dec 9, 2002
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $

 */
public abstract class TagLibrary {

  private Map tags = new HashMap();

  /**
   * Constructs a new TagLibrary and regisiters all factories.
   */
  public TagLibrary() {
    registerTags();
  }

  /**
   * Registers all factories for the TagLibrary.
   */
  abstract protected void registerTags();

  /**
   * Registers a class for the given tag name
   *
   * @param name <code>String</code> the tag's name
   * @param template <code>Class</code> the java class that represents the tag
   */
  public void registerTag( String name, Class template ) {
    registerTag( name, new DefaultFactory( template ) );
  }

  /**
   * Registers a factory for the given tag name
   *
   * @param name <code>String</code> the tag's name
   * @param factory <code>FactoryFactory</code> factory to create an Instance of the tag
   */
  public void registerTag( String name, Factory factory ) {
    tags.put( name.toLowerCase(), factory );
  }

  /**
   * Un-registers (removes) a registered tag.
   *
   * @param name <code>String</code> the tag's name
   * @return <code>boolean</code> true if tag was registered befoire and now successfuly removed.
   */
  public boolean unregisterTag( String name ) {
    return (null != tags.remove( name ));
  }

  /**
   * @return <code>Map</code> - all registered tags.
   * <pre>Use athe tag names to get to the factories</pre>
   */
  public Map getTagClasses() {
    return tags;
  }

  /**
   * Returns the Factory that is currently registered for the given Tag name
   * @param name <code>String</code>
   * @return <code>Factory</code> - regsitered for the given tag name
   */
  public Factory getFactory( String name ) {
    return (Factory) tags.get( name.toLowerCase() );
  }

  /**
   * Returns the Factory that is currently registered for the given Tag name
   * @param template <code>Class</code>
   * @return <code>Factory</code> - regsitered for the given tag name
   */
  public Factory getFactory( Class template ) {
    Factory factory = null;
    Iterator it = tags.values().iterator();
    while (it != null && it.hasNext()) {
      Factory f = (Factory) it.next();
      if (f.getTemplate().equals( template )) {
        factory = f;
        break;
      }
    }
    return factory;
  }

  /**
   * Returns a setter method by name for a specified template class
   * @param template <code>Class</code> template class
   * @param name <code>Sting</code> method name
   * @return <code>Method</code> - a setter method for the given class.
   * @see #guessSetter(Class, String)
   * @see org.swixml.Factory#getSetter(String)
   */
  protected Method getSetter( Class template, String name ) {
    Method method = null;
    Factory factory = getFactory( template.getName() );
    if (factory != null) {
      method = factory.getSetter( name );
    }
    return method;
  }

  /**
   * Returns a setter method by name for a specified template class
   * @param template <code>Class</code> template class
   * @param name <code>Sting</code> attribute name
   * @return <code>Method</code> - a setter method for the given class, able to modify the property.
   * @see #getSetter(Class, String)
   * @see org.swixml.Factory#guessSetter(String)
   */
  protected Method guessSetter( Class template, String name ) {
    Method method = null;
    Factory factory = getFactory( template.getName() );
    if (factory != null) {
      method = factory.guessSetter( name );
    }
    return method;
  }

}
