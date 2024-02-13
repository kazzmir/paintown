/*--
 $Id: Factory.java,v 1.1 2004/03/01 07:56:05 wolfpaulus Exp $

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

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Collection;

/**
 * An interface to represent a generic factory
 *
 * @author <a href="mailto:wolf@paulus.com">Wolf Paulus</a>
 * @version $Revision: 1.1 $
 */
public interface Factory {
  /** Specifies the prefix string for all setter methods */
  static final String SETTER_ID = "set";
  static final String ADDER_ID = "add";
  /**
   * Create a new component instance
   *
   * @return instance <code>Object</code> a new instance of a template class
   * @throws Exception
   */
  Object newInstance() throws Exception;

  /**
   * Creates a new Object which class is {@link #getTemplate()}
   * @param parameter <code>Object</code>, parameter used during construction or initialization.
   * @return instance <code>Object</code> a new instance of a template class
   * @throws Exception
   */
  Object newInstance( Object parameter ) throws Exception;

  /**
   * Creates a new Object which class is {@link #getTemplate()} and the constructor
   * parameter are <code>parameter</code>.
   *
   * @param parameter <code>Object[]</code> the parameter array to be passed into the constructor
   * @return <code>Object</object> - the created object, an instance of the template class
   * @throws InstantiationException if the creation of the object failed
   * @throws IllegalAccessException if the constructor is either private or protected.
   * @throws InvocationTargetException if the constructor invoked throws an exception
   */
  Object newInstance(Object[] parameter) throws InstantiationException, IllegalAccessException, InvocationTargetException;

  /**
   * @return class - <code>Class</code> the backing class template
   */
  Class getTemplate();

  /**
   * @return <code>Collection</code> - containing all available setter methods
   */
  Collection getSetters();

  /**
   * Returns a setter method, which accepts a parameter of the given type
   * @param template <code>Class</code> type of the setter method's parameter
   * @return <code>Method</code> setter method which maybe invoked on an object of the template class
   */
  Method getSetter( Class template );

  /**
   * Returns a setter method by name<br>
   * @param name <code>String</code> name of the setter method
   * @return <code>Method</code> - setter method which can be invoked on an object of the template class
   * @see #guessSetter
   * <pre><b>Typical Use:</b>
   * <p>Method method = factory.getSetter(&quot;set&quot; + Parser.capitalize(attr.getName()));</p>
   * </pre>
   */
  Method getSetter( String name );

  /**
   * Returns a setter method by a Attribute name.
   * Differently to the <code>getSetter</code> method, here the attibute name can be used directly and
   * case doesn't matter.<br>
   * @param name <code>String</code> name of the setter method
   * @return <code>Method</code> - setter method which can invoked on an object of the template class
   * @see #getSetter
   * <pre><b>Typical Use:</b>
   * <p>Method method = factory.getSetter( attr.getName() );</p>
   * </pre>
   */
  Method guessSetter(String name);

}
