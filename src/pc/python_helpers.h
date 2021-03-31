
#ifndef PYTHOH_HELPERS_H
#define PYTHOH_HELPERS_H

#define PYTHON_ENCAPSULE(obj, cap_type, logger, ...) \
    PyCapsule_New((void *)obj, cap_type, NULL); \
    if (PyErr_Occurred()) { \
        python_log_error(logger, \
            "during encapsule " cap_type " (%s, line %d):\n", \
            __FILE__, __LINE__); \
        PyErr_Print(); \
        __VA_ARGS__; \
    }

#define PYTHON_DECAPSULE(cap, cap_type, struct_type, logger, ...) \
    (struct_type *)PyCapsule_GetPointer((PyObject *)cap, \
        cap_type); \
    if (PyErr_Occurred()) { \
        PyErr_Print(); \
        python_log_error(logger, \
            "during decapsule " #struct_type " (%s, expected " \
                cap_type ") (%s, line %d)\n", \
            PyCapsule_GetName((PyObject *)cap), __FILE__, __LINE__); \
        if (PyErr_Occurred()) { \
            python_log_error(logger, \
                "during attempt to get capsule name (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
        } \
        __VA_ARGS__; \
    }

#define PYTHON_SET(var, var_type, cap_type, struct_type, py_type, py_getter, module, native_prop, logger) \
    static PyObject * \
    module ## _set_ ## var(py_type *self, PyObject *args) { \
        struct_type *owner = NULL; \
        var_type var; \
        var = (var_type)py_getter(args); \
        if (PyErr_Occurred()) { \
            python_log_error(logger, "during " #struct_type " set " #var " (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        owner = PYTHON_DECAPSULE(self->native_prop, cap_type, struct_type, logger, Py_RETURN_NONE); \
        owner->var = var; \
        Py_RETURN_NONE; \
    }

#define PYTHON_GET(var, cap_type, struct_type, py_type, c_getter, module, native_prop, logger) \
    static PyObject * \
    module ## _get_ ## var(const py_type *self) { \
        struct_type *owner = NULL; \
        PyObject *var; \
        owner = PYTHON_DECAPSULE(self->native_prop, cap_type, struct_type, logger, Py_RETURN_NONE); \
        var = c_getter(owner->var); \
        if (PyErr_Occurred()) { \
            python_log_error(logger, "during " #struct_type " get " #var " (%s, line %d)\n", \
                __FILE__, __LINE__); \
            PyErr_Print(); \
            Py_RETURN_NONE; \
        } \
        /* No Py_INCREF because the 1 ref from c_getter is sufficient. */ \
        return var; \
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

#define PYTHON_WRAP_NOARGS(self_type, cap_type, struct_type, fnc, logger) \
    static PyObject * \
    PyMario_ ## fnc(self_type *self) { \
        struct_type *var = NULL; \
        var = PYTHON_DECAPSULE(self->native_state, cap_type, struct_type, logger, Py_RETURN_NONE); \
        fnc( var ); \
        Py_RETURN_NONE; \
    }

#define PYTHON_ADD_CONSTANT(module, constant) \
    PyModule_AddIntConstant(module, #constant, constant);

#endif /* PYTHOH_HELPERS_H */
