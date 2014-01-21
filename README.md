OpenGL Objects Wrapper Library
====

The OpenGL Objects Wrapper Library (glow) provides an object oriented C++ interface for OpenGL's programmable pipeline (3.0+).
glow is a cross-platform library licenced under the [MIT license](http://opensource.org/licenses/MIT).

The current pre-release is [glow-v0.2.0](https://github.com/hpicgs/glow/releases/tag/glow-v0.2.0).
To find out more about glow and how to use it, check out our [wiki](https://github.com/hpicgs/glow/wiki).


### Documentation

A first basic glow documentation can be found at http://costumebrother.de/glow.


### Dependencies

The following dev-libraries and programs need to be provided for correct CMake configuration:
* C++11 compatible compiler (e.g. gcc 4.7, MSVC 2013)
* CMake (>=2.8.9, better 2.8.12): http://www.cmake.org/
* OpenGL Extension Wrangler (GLEW, >=1.10.0): http://glew.sourceforge.net/, https://github.com/nigels-com/glew
* OpenGL Mathematics (GLM, >=0.9.3): http://glm.g-truc.net/
* Window and Context creation (GLFW 3.0.3): http://www.glfw.org/ (needed for the now mandatory glowwindow module)


## Development Notes

If you are contributing to this project, please keep the following notes in mind:
* Add your name and email to the AUTHORS file.
* Follow coding conventions according to google's [C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml).
* Commits should provide a meaningful  message, which uses the imperative, present tense: "change", not "changed" nor "changes", AND start with a capital letter.
* Commits should always refer to an issue: use ```#xxx```, ```fix #xxx```, or ```close #xxx```.
* Pull Requests are reviewed by at least one other developer on another platform.
* Use lazy initialization as often as possible for time consuming tasks.
* Member Variables always start with ```m_```, ```g_```, and ```s_``` (e.g.,```m_member```, ```g_global```, and ```s_static```)..
* Interface or abstract class names always start with Abstract (e.g., ```AbstractArray```).
* Enforce strict include sequence: gl, glew, std, glm, header, glow (there should be only a few exceptions).
