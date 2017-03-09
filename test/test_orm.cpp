//ODB: C++ Object-Relational Mapping (ORM)

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <initializer_list>
#include <memory>
#include <tinyreflection.h>

#include "tinyorm.h"
#include "tinylogger.h"

////////////////////////////////////////////////////////////////
//
// C++结构体存储到MYSQL
//
////////////////////////////////////////////////////////////////
template<typename T, typename T_DBDescriptor>
struct Object2DB : public T {
public:
    using T::T;

    typedef std::vector<T> Records;

    Object2DB() {}

    Object2DB(const T &object) : T(object) {}

    //
    // 创建、删除、自动更新表结构
    //
    static bool createTable();

    static bool dropTable();

    static bool updateTable();

    //
    // 数据库批量操作
    //
    static bool loadFromDB(Records &records, const char *clause, ...);

    static bool deleteFromDB(const char *where, ...);

    template<typename T_Set>
    static bool loadFromDB(T_Set &records, const char *clause, ...);

protected:
    // 数据库描述
    static T_DBDescriptor descriptor_;

public:
    //
    // 该对象的数据库操作
    //
    bool selectDB();

    bool insertDB();

    bool replaceDB();

    bool updateDB();

    bool deleteDB();
};


struct PoDMem {
    uint32_t value;
};

struct NonPoDMem {
    uint32_t id;
    std::string name;
};

//template <>
//struct TinySerializer<NonPoDMem> {
//    static bool serialize(const NonPoDMem &object, std::string &bin) {
//        bin = "NonPoDMem";
//        return true;
//    }
//
//    static bool deserialize(NonPoDMem &object, const std::string &bin) {
//        return true;
//    }
//};

struct Player {
    uint32_t id = 1024;
    std::string name = "david";
    uint8_t age = 30;

    PoDMem obj1;
    NonPoDMem obj2;
};


// TODO: 生成DB操作
struct PlayerRegister {
    PlayerRegister() {
        StructFactory::instance().declare<NonPoDMem>()
                .property("id", &NonPoDMem::id)
                .property("name", &NonPoDMem::name);

        TableFactory::instance().table<Player>("PLAYER")
                .field(&Player::id, "ID", FieldType::UINT32)
                .field(&Player::name, "NAME", FieldType::VCHAR, "david", 32)
                .field(&Player::age, "AGE", FieldType::UINT8, "30")
                .field(&Player::obj1, "OBJ1", FieldType::OBJECT)
                .field(&Player::obj2, "OBJ2", FieldType::OBJECT)
                .key("ID")
                .index("NAME");
    }
};

static PlayerRegister register___;



typedef std::unordered_map<uint32_t, Player> PlayerSet;

//#include <soci.h>
//#include <soci-mysql.h>
//
//template<typename T>
//void insert(TableDescriptorBase::Ptr tdbase, T &obj) {
//
//    TableDescriptor<T> *td = (TableDescriptor<T> *) tdbase.get();
//    try {
//        soci::session sql(soci::mysql, "host=127.0.0.1 db=tinyworld user=david password='123456'");
//
//        std::ostringstream os;
//        os << "INSERT INTO `" << td->table
//           << "`(" << td->sql_fieldlist() << ")"
//           << " VALUES (" << td->sql_fieldlist2() << ")";
//
//        std::cout << os.str() << std::endl;
//
//
//        for (auto fd : td->fieldIterators()) {
//
//            if (fd->type == FieldType::UINT32)
//                std::cout << fd->name << " : " << td->reflection.template get<uint32_t>(obj, fd->name) << std::endl;
//            else if (fd->type == FieldType::VCHAR)
//                std::cout << fd->name << " : " << td->reflection.template get<std::string>(obj, fd->name) << std::endl;
//            else if (fd->type == FieldType::UINT8)
//                std::cout << fd->name << " : " << (int) td->reflection.template get<uint8_t>(obj, fd->name)
//                          << std::endl;
//            else
//                std::cout << fd->name << ": unkown" << std::endl;
//        }
//
//
//        soci::statement st(sql);
//
//        for (auto fd : td->fieldIterators()) {
//
//            if (fd->type == FieldType::UINT32)
//                st.exchange(soci::use(td->reflection.template get<uint32_t>(obj, fd->name)));
//            else if (fd->type == FieldType::VCHAR)
//                st.exchange(soci::use(td->reflection.template get<std::string>(obj, fd->name)));
//            else if (fd->type == FieldType::UINT8)
//                st.exchange(soci::use(td->reflection.template get<uint8_t>(obj, fd->name)));
//            else if (fd->type == FieldType::OBJECT) {
//                std::string bin;
//                auto prop = td->reflection.getPropertyByName(fd->name);
//                if (prop)
//                    prop->to_bin(obj, bin);
//
//                st.exchange(soci::use(bin));
//
//                std::cout << bin << std::endl;
//            }
//        }
//
//
//        st.alloc();
//        st.prepare(os.str());
//        st.define_and_bind();
//        st.execute(true);
//
////        sql << "insert into Player(id, name, age) values(:id, :name, :age)", soci::use(p);
//
//        std::cout << "--- end --" << std::endl;
//
//    }
//    catch (const soci::mysql_soci_error &e) {
//        std::cout << "MySQL Error:" << e.what() << std::endl;
//    }
//    catch (const std::exception &e) {
//        std::cout << "Error:" << e.what() << std::endl;
//    }
//}





// TODO: 自动生成
#include "soci.h"
#include "soci-mysql.h"

namespace soci {

    template<>
    struct type_conversion<Player> {
        typedef values base_type;

        static void from_base(const values &v, indicator ind, Player &p) {
            p.id = v.get<uint32_t>("id");
            p.name = v.get<std::string>("name");
            p.age = v.get<uint8_t>("age");
        }

        static void to_base(const Player &p, values &v, indicator &ind) {
            v.set("id", p.id);
            v.set("name", p.name);
            v.set("age", p.age);
            ind = i_ok;
        }
    };
}


#include "tinyorm_mysql.h"



void test_mysql() {
    MySqlConnectionPool::instance()->setServerAddress("mysql://david:123456@127.0.0.1/tinyworld?maxconn=5");
    MySqlConnectionPool::instance()->createAll();
    MySqlConnectionPool::instance()->setGrabWaitTime(0);

    TinyORM db;
    db.updateTables();
//    db.dropTable("PLAYER");
//    db.createTable("PLAYER");
//    db.updateTable("PLAYER");

    return;

    for (uint32_t i = 0; i < 10; ++ i) {
        LOG_DEBUG(__FUNCTION__, "..........");

        Player p;
        p.id = i + 10000;
        p.age = i;

        db.insert(p);
//        db.test();

//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void test_soci() {

}

int main() {
    auto td = TableFactory::instance().getTableDescriptor("PLAYER");
    std::cout << td->sql_create() << std::endl;
    std::cout << td->sql_drop() << std::endl;
    std::cout << td->sql_addfield("NAME") << std::endl;
    test_mysql();
    return 0;


//
//    for (uint32_t i = 0; i < 10; ++i) {
//        Player p;
//        p.id = i + 10000;
//        p.age = i;
////        insert(td, p);
//        insert_mysqlpp(td, p);
//
//        std::string bin;
//    }
//
//    std::cout << std::is_trivial<Player>::value << std::endl;
}