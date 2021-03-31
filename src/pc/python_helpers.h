
#ifndef PYTHOH_HELPERS_H
#define PYTHOH_HELPERS_H

#define PYTHON_ENCAPSULE(obj, cap_type, ...) \
    PyCapsule_New((void *)obj, cap_type, NULL); \
    if (PyErr_Occurred()) { \
        fprintf(stderr, \
            "during encapsule " cap_type " (%s, line %d):\n", \
            __FILE__, __LINE__); \
        PyErr_Print(); \
        __VA_ARGS__; \
    }
    
#define PYTHON_DECAPSULE(cap, cap_type, struct_type, ...) \
    (struct_type *)PyCapsule_GetPointer((PyObject *)cap, \
        cap_type); \
    if (PyErr_Occurred()) { \
        PyErr_Print(); \
        fprintf(stderr, \
            "during decapsule " #struct_type " (%s, expected " \
                cap_type ") (%s, line %d)\n", \
            PyCapsule_GetName((PyObject *)cap), __FILE__, __LINE__); \
        if (PyErr_Occurred()) { \
            fprintf(stderr, \
                "during attempt to get capsule name (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
        } \
        __VA_ARGS__; \
    }

#define PYTHON_WRAP_FLAGS(flag_name, flag_var, flag_module, logger) \
    static PyObject * \
    flag_module ## _unset_ ## flag_name(PyObject *self, PyObject *arg) { \
        u32 flag_name; \
        flag_name = PyLong_AsUnsignedLong( arg ); \
        if (PyErr_Occurred()) { \
            python_log_error(logger, "during unset flag:"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        flag_var &= ~flag_name; \
        Py_RETURN_NONE; \
    } \
    \
    static PyObject * \
    flag_module ## _set_ ## flag_name(PyObject *self, PyObject *arg) { \
        u32 flag_name; \
        flag_name = PyLong_AsUnsignedLong( arg ); \
        if (PyErr_Occurred()) { \
            python_log_error(logger, "during unset flag:"); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        flag_var |= flag_name; \
        Py_RETURN_NONE; \
    }

#endif /* PYTHOH_HELPERS_H */
