/****************************************************************************
** Meta object code from reading C++ file 'tsp_evolution.h'
**
** Created: Tue Mar 30 16:58:00 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "tsp_evolution.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tsp_evolution.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TSPEvolution[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // signals: signature, parameters, type, tag, flags
      98,   14,   13,   13, 0x05,
     165,  154,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TSPEvolution[] = {
    "TSPEvolution\0\0"
    "resultPath,actGeneration,minCurrentPathPrice,avgCurrentPathPrice,axCur"
    "rentPathPrice\0"
    "CurrentPathGenerated(tIndividual,int,float,float,float)\0"
    "resultPath\0PathGenerated(tIndividual)\0"
};

const QMetaObject TSPEvolution::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_TSPEvolution,
      qt_meta_data_TSPEvolution, 0 }
};

const QMetaObject *TSPEvolution::metaObject() const
{
    return &staticMetaObject;
}

void *TSPEvolution::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TSPEvolution))
        return static_cast<void*>(const_cast< TSPEvolution*>(this));
    return QThread::qt_metacast(_clname);
}

int TSPEvolution::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: CurrentPathGenerated((*reinterpret_cast< tIndividual(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
        case 1: PathGenerated((*reinterpret_cast< tIndividual(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TSPEvolution::CurrentPathGenerated(tIndividual _t1, int _t2, float _t3, float _t4, float _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TSPEvolution::PathGenerated(tIndividual _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
