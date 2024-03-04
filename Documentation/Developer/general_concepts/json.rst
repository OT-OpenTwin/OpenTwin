JSON
====

OpenTwin provides a JSON API which wraps the ``rapidJSON`` library.
Classes that may be serialized should inherit from *ot::Serializable* (``OTCore/Serializable.h``).

Full Example
------------

Header
^^^^^^

.. code-block:: c++
    :caption: YourSource.h

    // OpenTwin header
    #include "OTCore/Serializable.h"

    // std header
    #include <list>
    #include <string>

    class A : public ot::Serializable {
    public:
        // Here the data is added to the provided JSON object (serialized)
        virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

        // Here the data is read from the provided JSON object (deserialized)
        virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

        void setX(int _x) { m_x = _x; };
        void addY(double _y) { m_y.push_back(_y); };

    private:
        int               m_x;
        std::list<double> m_y;
    };

    class B : public ot::Serializable {
    public:
        // Here the data is added to the provided JSON object (serialized)
        virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;
        
        // Here the data is read from the provided JSON object (deserialized)
        virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

        void addA(const A& _a) { m_a.push_back(_a); };
        void setB(const std::string& _b) { m_b = _b; };

    private:
        std::list<A> m_a;
        std::strnig  m_b;

    }

Source
^^^^^^

.. code-block:: c++
    :caption: YourSource.cpp

    // Project header
    #include "YourHeader.h"
    
    // Class A

    // Here the data is added to the provided JSON object (serialized)
    void A::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
        _object.AddMember("X", m_x, _allocator);
        // The JsonArray provides multiple constructors for lists and vectors of all base datatypes
        _object.AddMember("Y", ot::JsonArray(m_y, _allocator), _allocator);
    }

    // Here the data is read from the provided JSON object (deserialized)
	void A::setFromJsonObject(const ot::ConstJsonObject& _object) {
        m_x = ot::JSON::getInt(_object, "X");
        m_y = ot::JSON::getDoubleList(_object, "Y");
    }

    // Class B

    // Here the data is added to the provided JSON object (serialized)
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
    
    // Here the data is read from the provided JSON object (deserialized)
	void B::setFromJsonObject(const ot::ConstJsonObject& _object) {
        // Get the A array
        ot::ConstJsonObjectList aArr = ot::JSON::getObjectList(_object, "A");

        // Iterate trough the array
        for (JsonSizeType i = 0; i < aArr.Size(); i++) {
            // Get the object
            ConstJsonObject aObj = JSON::getObject(pArr, i);

            // Create new Entry
            A a;
            a.setFromJsonObject(aObj);
            m_a.push_back(a);
        }

        m_b = ot::JSON::getString(_object, "B");
    }

.. code-block:: c++
    :caption: main.cpp

    // Project header
    #include "YourHeader.h"
    
    // OpenTwin header
    #include "OTCommunication/ActionTypes.h"

    int main(int _argc, char** _argv) {
        // Create JSON document (object by default)
        ot::JsonDocument doc;

        // Create data to serialize

        // Frist create the child items
        A a1;
        a1.setX(1);
        a1.addY(10.f);
        a1.addY(20.f);

        A a2;
        a2.setX(2);
        a2.addY(30.f);
        a2.addY(40.f);

        // Create the root item and add the childs
        B b;
        b.addA(a1);
        b.addA(a2);
        b.setB("Test");

        // Now we could create a request for a imaginary service
        doc.AddMember(OT_ACTION_MEMBER, JsonString(<Your action>, doc.GetAllocator()), doc.GetAllocator());
        
        // Create a JsonObject for the root item and serialize it
        ot::JsonObject obj;
        b.addToJsonObject(obj, doc.GetAllocator());

        // Add the object to the request document
        doc.AddMember(<Your parameter name>(e.g. OT_ACTION_PARAM_Package), obj, doc.GetAllocator());

        // Generate JSON
        std::string json = doc.toJson();

        // Send the request ...
        
        // When receiving the request:

        // Create empty JSON document for import
        ot::JsonDocument impDoc;

        // Parse JSON
        impDoc.fromJson(json);

        // Get the package
        ot::ConstJsonObject impObj = ot::json::getObject(impDoc, <Your parameter name>(e.g. OT_ACTION_PARAM_Package));

        // Create import root object
        B bImp;

        // Set the data from the JSON object
        bImp.setFromJsonObject(impObj);

        return 0;
    }