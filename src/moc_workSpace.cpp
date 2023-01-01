/****************************************************************************
** Meta object code from reading C++ file 'workSpace.h'
**
** Created: Tue Mar 30 16:58:01 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "workSpace.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'workSpace.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WorkSpace[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      29,   10,   10,   10, 0x08,
      45,   10,   10,   10, 0x08,
      62,   10,   10,   10, 0x08,
      78,   10,   10,   10, 0x08,
     104,   95,   10,   10, 0x08,
     127,   95,   10,   10, 0x08,
     151,   95,   10,   10, 0x08,
     175,   95,   10,   10, 0x08,
     196,   95,   10,   10, 0x08,
     218,   95,   10,   10, 0x08,
     238,   95,   10,   10, 0x08,
     261,   95,   10,   10, 0x08,
     369,  284,   10,   10, 0x08,
     443,  432,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WorkSpace[] = {
    "WorkSpace\0\0SetOpenFileName()\0"
    "SetResultFile()\0StartEvolution()\0"
    "StopEvolution()\0AbortEvolution()\0"
    "newValue\0ChangeSPopulation(int)\0"
    "ChangeNGenerations(int)\0ChangePMutation(double)\0"
    "ChangeTMutation(int)\0ChangeTCrossover(int)\0"
    "ChangeSElitism(int)\0ChangeSTournament(int)\0"
    "ChangeLogInterval(int)\0"
    "resultPath,actGeneration,minCurrentPathPrice,avgCurrentPathPrice,maxCu"
    "rrentPathPrice\0"
    "ReceiveCurrentPathGenerated(tIndividual,int,float,float,float)\0"
    "resultPath\0ReceivePathGenerated(tIndividual)\0"
};

const QMetaObject WorkSpace::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_WorkSpace,
      qt_meta_data_WorkSpace, 0 }
};

const QMetaObject *WorkSpace::metaObject() const
{
    return &staticMetaObject;
}

void *WorkSpace::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WorkSpace))
        return static_cast<void*>(const_cast< WorkSpace*>(this));
    return QDialog::qt_metacast(_clname);
}

int WorkSpace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: SetOpenFileName(); break;
        case 1: SetResultFile(); break;
        case 2: StartEvolution(); break;
        case 3: StopEvolution(); break;
        case 4: AbortEvolution(); break;
        case 5: ChangeSPopulation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: ChangeNGenerations((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: ChangePMutation((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: ChangeTMutation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: ChangeTCrossover((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: ChangeSElitism((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: ChangeSTournament((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: ChangeLogInterval((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: ReceiveCurrentPathGenerated((*reinterpret_cast< tIndividual(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3])),(*reinterpret_cast< float(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5]))); break;
        case 14: ReceivePathGenerated((*reinterpret_cast< tIndividual(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
