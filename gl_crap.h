
GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar strInfoLog[infoLogLength];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        printf("compilation error in shader %s\n", strInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    printf("success! (type: %d)\n", type);
    return shader;
}

int compile_shader_program(const char* str_vert_shader, const char* str_frag_shader, const char* attrib_name_0, const char* attrib_name_1) {
    GLuint vert_shader;
    GLuint frag_shader;
    GLuint prog_object;

    vert_shader = compile_shader(GL_VERTEX_SHADER, str_vert_shader);
    if(vert_shader == 0) {
        printf("error compiling vert shader\n");
        return 1;
    }

    frag_shader = compile_shader(GL_FRAGMENT_SHADER, str_frag_shader);
    if(frag_shader == 0) {
        printf("error compiling frag shader\n");
        return 1;
    }

    printf("creating shader program\n");

    prog_object = glCreateProgram();
    glAttachShader(prog_object, vert_shader);
    glAttachShader(prog_object, frag_shader);

    if (attrib_name_0 != NULL) {
        printf("binding attrib 0 with name: %s\n", attrib_name_0);
        glBindAttribLocation(prog_object, 0, attrib_name_0);
    }

    if (attrib_name_1 != NULL) {
        printf("binding attrib 1 with name: %s\n", attrib_name_1);
        glBindAttribLocation(prog_object, 1, attrib_name_1);
    }

    printf("linking shader program");
    glLinkProgram(prog_object);

    return prog_object;
}