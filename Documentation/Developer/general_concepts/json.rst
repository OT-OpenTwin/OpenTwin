JSON
====

OpenTwin provides a JSON API which wraps the ``rapidjson`` library.
Classes that may be serialized should inherit from *ot::Serializable* (``OTCore/Serializable.h``).

-------
Example
-------

``YourHeader.h``

.. code-block:: c++

    // OpenTwin header
    #include "OTCore/Serializable.h"

    // std header
    #include <list>
    #include <string>

    class A : public ot::Serializable {
    public:
        virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
        virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

        void setX(int _x) { m_x = _x; };
        void addY(double _y) { m_y.push_back(_y); };

    private:
        int               m_x;
        std::list<double> m_y;
    };

    class B : public ot::Serializable {
    public:
        virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
        virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

        void addA(const A& _a) { m_a.push_back(_a); };
        void setB(const std::string& _b) { m_b = _b; };

    private:
        std::list<A> m_a;
        std::strnig  m_b;

    }

``YourSource.cpp``

.. code-block:: c++

    // Project header
    #include "YourHeader.h"
    
    void A::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
        _object.AddMember("X", m_x, _allocator);
        // The JsonArray provides multiple constructors for lists and vectors of all base datatypes
        _object.AddMember("Y", ot::JsonArray(m_y, _allocator), _allocator);
    }

	void A::setFromJsonObject(const ot::ConstJsonObject& _object) {
        m_x = ot::json::getInt(_object, "X");
        m_y = ot::json::getDoubleList(_object, "Y");
    }

    void B::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
        // Create empty array for the A entries
        ot::JsonArray aArr;
        for (const A& a : m_a) {
            // Create empty object for each A entry
            ot::JsonObject aObj;

            // Let the entry write its data to the object
            a.addToJsonObject(aObj, _allocator);

            // Push the entry to the array
            arr.PushBack(aObj, _allocator);
        }

        // Add the array to the provided object
        _object.AddMember("A", aArr, _allocator);

        // The JsonString should be used when adding std or c strings to an array or object
        _object.AddMember("B", ot::JsonString(m_b, _allocator), _allocator);
    }

	void B::setFromJsonObject(const ot::ConstJsonObject& _object) {
        // Get the A array
        ot::ConstJsonObjectList aArr = ot::json::getObjectList(_object, "A");

        // Iterate trough the array
        for (const ot::ConstJsonObject& aObj : aArr) {
            // Create new Entry
            A a;
            a.setFromJsonObject(aObj);
            m_a.push_back(a);
        }

        m_b = ot::json::getString(_object, "B");
    }

``main.cpp``

.. code-block:: c++

    // Project header
    #include "YourHeader.h"

    int main(int _argc, char** _argv) {
        // Create data to serialize
        A a1;
        a1.setX(1);
        a1.addY(10.f);
        a1.addY(20.f);

        A a2;
        a2.setX(2);
        a2.addY(30.f);
        a2.addY(40.f);

        B b;
        b.addA(a1);
        b.addA(a2);
        b.setB("Test");

        // Create JSON document (object by default)
        ot::JsonDocument doc;
        
        // Let the root object seriaze to the document
        b.addToJsonObject(doc, doc.GetAllocator());

        // Generate json
        std::string json = doc.toJson();

        // Create empty json document for import
        ot::JsonDocument impDoc;

        // Read json
        impDoc.fromJson(json);

        // Create import root object
        B bImp;

        // Set the data from the json object
        bImp.setFromJsonObject(impDoc);

        return 0;
    }