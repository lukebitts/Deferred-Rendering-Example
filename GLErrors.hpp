//
//  Vertex.h
//  OpenGLTest
//
//  Created by Lucas Bittencourt on 8/9/13.
//  Copyright (c) 2013 Lucas Bittencourt. All rights reserved.
//

#ifndef _GLERRORS_H_
#define _GLERRORS_H_

#ifdef DEBUG

#include <iostream>

#define DETECT_ERROR_AND_CORRECT(CODE) { int err; CODE, ((err = glGetError()) != 0) ? (std::cout<<err<<" ("<<__FILE__<<" "<<__LINE__<<")"<<std::endl) : std::cout<<"0 "<<__FILE__<<" "<<__LINE__<<std::endl; }
#define DETECT_ERROR(CODE) { int err; CODE, ((err = glGetError()) != 0) ? (std::cout<<err<<" ("<<__FILE__<<" "<<__LINE__<<")"<<std::endl) : std::cout<<""; }

#define glGenRenderbuffersEXT(...) DETECT_ERROR(glGenRenderbuffersEXT(__VA_ARGS__))
#define glBindFramebufferEXT(...) DETECT_ERROR(glBindFramebufferEXT(__VA_ARGS__))
#define glBindRenderbufferEXT(...) DETECT_ERROR(glBindRenderbufferEXT(__VA_ARGS__))
#define glRenderbufferStorageEXT(...) DETECT_ERROR(glRenderbufferStorageEXT(__VA_ARGS__))
#define glFramebufferRenderbufferEXT(...) DETECT_ERROR(glFramebufferRenderbufferEXT(__VA_ARGS__))
#define glGenTextures(...) DETECT_ERROR(glGenTextures(__VA_ARGS__))
#define glBindTexture(...) DETECT_ERROR(glBindTexture(__VA_ARGS__))
#define glTexImage2D(...) DETECT_ERROR(glTexImage2D(__VA_ARGS__))
#define glFramebufferTexture2DEXT(...) DETECT_ERROR(glFramebufferTexture2DEXT(__VA_ARGS__))
#define glDrawBuffers(...) DETECT_ERROR(glDrawBuffers(__VA_ARGS__))
#define glClearColor(...) DETECT_ERROR(glClearColor(__VA_ARGS__))
#define glClear(...) DETECT_ERROR(glClear(__VA_ARGS__))
#define glUseProgram(...) DETECT_ERROR(glUseProgram(__VA_ARGS__))
#define glUniformMatrix4fv(...) DETECT_ERROR(glUniformMatrix4fv(__VA_ARGS__))
#define glUniformMatrix3fv(...) DETECT_ERROR(glUniformMatrix3fv(__VA_ARGS__))
#define glEnableVertexAttribArray(...) DETECT_ERROR(glEnableVertexAttribArray(__VA_ARGS__))
#define glBindBuffer(...) DETECT_ERROR(glBindBuffer(__VA_ARGS__))
#define glVertexAttribPointer(...) DETECT_ERROR(glVertexAttribPointer(__VA_ARGS__))
#define glDrawElements(...) DETECT_ERROR(glDrawElements(__VA_ARGS__))
#define glDisableVertexAttribArray(...) DETECT_ERROR(glDisableVertexAttribArray(__VA_ARGS__))
#define glActiveTexture(...) DETECT_ERROR(glActiveTexture(__VA_ARGS__))
#define glBindTexture(...) DETECT_ERROR(glBindTexture(__VA_ARGS__))
#define glGenerateMipmapEXT(...) DETECT_ERROR(glGenerateMipmapEXT(__VA_ARGS__))
#define glAttachShader(...) DETECT_ERROR(glAttachShader(__VA_ARGS__))
#define glLinkProgram(...) DETECT_ERROR(glLinkProgram(__VA_ARGS__))
#define glDeleteProgram(...) DETECT_ERROR(glDeleteProgram(__VA_ARGS__))
#define glUniform1i(...) DETECT_ERROR(glUniform1i(__VA_ARGS__))
#define glBlitFramebufferEXT(...) DETECT_ERROR(glBlitFramebufferEXT(__VA_ARGS__))



#endif

#endif
