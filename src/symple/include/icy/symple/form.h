///
//
// Icey
// Copyright (c) 2005, Icey <https://0state.com>
//
// SPDX-License-Identifier: LGPL-2.1+
//
/// @addtogroup symple
/// @{


#pragma once


#include "icy/symple/command.h"
#include "icy/symple/symple.h"
#include <string_view>


namespace icy {
namespace smpl {


class Symple_API Form;


class Symple_API FormField;
/// Base element within a Symple form.
///
/// FormElement wraps a reference to an external JSON node and provides
/// typed accessors for the common `type`, `id`, `label`, and `elements`
/// fields. Pages, sections, and fields all derive from this base.
class Symple_API FormElement
{
public:
    /// Constructs an unbound element (root pointer is null).
    FormElement();

    /// Constructs an element bound to the given JSON node.
    /// @param root  JSON node this element refers to.
    /// @param type  Optional element type string.
    /// @param id    Optional element ID string.
    /// @param label Optional display label string.
    FormElement(json::Value& root, std::string_view type = "",
                std::string_view id = "", std::string_view label = "");

    /// Copy constructor; copies the root pointer reference (shallow).
    /// @param r Source element.
    FormElement(const FormElement& r);

    /// Copy-assigns the root pointer reference.
    /// @param r Source element.
    FormElement& operator=(const FormElement& r);

    virtual ~FormElement();

    /// Returns the element type string.
    [[nodiscard]] std::string type() const;

    /// Returns the element ID string.
    [[nodiscard]] std::string id() const;

    /// Returns the display label string.
    [[nodiscard]] std::string label() const;

    /// Sets the element type.
    /// Possible values: page, section, text, text-multi,
    ///      list, list-multi, checkbox, media, custom
    /// @param type Element type string.
    void setType(std::string_view type);

    /// Sets the element ID field.
    /// @param id Element ID string.
    void setId(std::string_view id);

    /// Sets the display label field.
    /// @param text Label text.
    void setLabel(std::string_view text);

    /// Sets the hint/description field shown below the element.
    /// @param text Hint text.
    void setHint(std::string_view text);

    /// Sets an optional validation error message.
    /// @param error Error text to display.
    void setError(std::string_view error);

    /// Appends a page child element and returns a handle to it.
    /// @param id    Optional page ID.
    /// @param label Optional page label.
    /// @return FormElement referencing the new page node.
    FormElement addPage(std::string_view id = "",
                        std::string_view label = "");

    /// Appends a section child element and returns a handle to it.
    /// @param id    Optional section ID.
    /// @param label Optional section label.
    /// @return FormElement referencing the new section node.
    FormElement addSection(std::string_view id = "",
                           std::string_view label = "");

    /// Appends a typed field child element and returns a handle to it.
    /// Throws std::invalid_argument if type is not a recognised field type.
    /// @param type  Field type (e.g. "text", "list", "boolean").
    /// @param id    Optional field ID.
    /// @param label Optional field label.
    /// @return FormField referencing the new field node.
    FormField addField(std::string_view type, std::string_view id = "",
                       std::string_view label = "");

    /// Searches child elements for the field with the given ID.
    /// @param id      Field ID to search for.
    /// @param partial If true, performs a substring match.
    /// @return FormField handle (may be invalid if not found).
    FormField getField(std::string_view id, bool partial = false);

    /// Populates a FormField by searching child elements for the given ID.
    /// @param id      Field ID to search for.
    /// @param field   Output field handle.
    /// @param partial If true, performs a substring match.
    /// @return True if the field was found.
    bool getField(std::string_view id, FormField& field,
                  bool partial = false);

    /// Returns true if any child element has an ID matching the given value.
    /// @param id      ID string to search for.
    /// @param partial If true, a substring match is sufficient.
    bool hasField(std::string_view id, bool partial = false);

    /// Sets the live flag on this element.
    /// Live elements are used to submit partial form sections
    /// (e.g. for auto-complete) without sending the entire form.
    /// @param flag True to enable live updates.
    void setLive(bool flag);

    /// Returns true if this field is live, meaning the
    /// form-processing entity should auto-update this
    /// field's value whenever it changes.
    [[nodiscard]] bool live() const;

    /// Removes all child elements whose ID matches the given value.
    /// @param id      ID string to match against.
    /// @param partial If true, removes elements whose ID contains the string.
    /// @return True if at least one element was removed.
    bool clearElements(std::string_view id, bool partial = false);

    /// Clears all fields from the underlying JSON node.
    void clear();

    /// Returns true if the form element is valid.
    [[nodiscard]] bool valid() const;

    /// Returns the number of child elements.
    int numElements();

    /// Returns true if any fields have errors.
    bool hasErrors();

    /// Returns true if the form has multiple pages.
    bool hasPages();

    /// Returns a reference to the underlying JSON node.
    /// Throws std::runtime_error if the root pointer is null.
    [[nodiscard]] json::Value& root() const;

protected:
    /// The root pointer is just a reference to
    /// the externally managed JSON value memory.
    json::Value* _root;
};


/// Interactive form with pages, sections, and fields for command data exchange.
///
/// Attach a Form to a Command message to carry structured input/output.
/// The `action` field governs the exchange direction; `partial` enables
/// live/auto-complete field submission.
class Symple_API Form : public FormElement
{
public:
    /// Constructs an unbound form (root pointer is null).
    Form();

    /// Constructs a form bound to the given JSON node.
    /// Sets the `type` field to "form".
    /// @param root JSON node to bind to.
    Form(json::Value& root);

    /// Constructs a form from a Command, binding to `command["form"]`.
    /// Sets the `type` field to "form".
    /// @param root Parent command message.
    Form(Command& root);

    virtual ~Form();

    /// Returns true if the element is valid, non-empty, and has no errors.
    bool valid();

    /// Returns the form action string (defaults to "form").
    [[nodiscard]] std::string action() const;

    /// Returns true if this is a partial form submission.
    [[nodiscard]] bool partial() const;

    /// Sets the form action field.
    /// Throws std::invalid_argument for unrecognised values.
    /// Possible values:
    ///   `form`   - Form-processing entity requests form completion.
    ///   `submit` - Form-submitting entity is sending data.
    ///   `cancel` - Form-submitting entity cancelled submission.
    ///   `result` - Form-processing entity is returning data.
    /// @param action Action string.
    void setAction(std::string_view action);

    /// Marks the form as a partial section for live/auto-complete updates.
    /// Partial forms transmit only the changed fields rather than
    /// the entire form payload.
    /// @param flag True to mark as partial.
    void setPartial(bool flag);
};


/// Form input field with typed values and selectable options.
///
/// Values are stored as strings in a JSON array. Typed accessors
/// (intValue, doubleValue, boolValue) parse the first string on demand.
class Symple_API FormField : public FormElement
{
public:
    /// Constructs an unbound field (root pointer is null).
    FormField();

    /// Constructs a field bound to the given JSON node.
    /// @param root  JSON node to bind to.
    /// @param type  Field type string (e.g. "text", "list").
    /// @param id    Optional field ID.
    /// @param label Optional display label.
    FormField(json::Value& root, std::string_view type = "",
              std::string_view id = "", std::string_view label = "");
    virtual ~FormField();

    /// Adds a labelled option for list-based fields.
    /// @param key   Option key sent on submit.
    /// @param value Display label for the option.
    void addOption(std::string_view key, std::string_view value);

    /// Adds an option whose key and display value are identical.
    /// @param value Option string.
    void addOption(std::string_view value);

    /// Replaces all current values with a single string value.
    /// @param value String value to set.
    void setValue(std::string_view value);

    /// Replaces all current values with a single integer value.
    /// @param value Integer value to set.
    void setValue(int value);

    /// Replaces all current values with a single double value.
    /// @param value Double value to set.
    void setValue(double value);

    /// Replaces all current values with a single boolean value.
    /// @param value Boolean value to set.
    void setValue(bool value);

    /// Appends a string value to the values array.
    /// @param value String value to append.
    void addValue(std::string_view value);

    /// Appends an integer value to the values array.
    /// @param value Integer value to append.
    void addValue(int value);

    /// Appends a double value to the values array.
    /// @param value Double value to append.
    void addValue(double value);

    /// Appends a boolean value to the values array.
    /// @param value Boolean value to append.
    void addValue(bool value);

    /// Returns a reference to the JSON array of all values.
    json::Value& values();

    /// Returns the first value as a string.
    /// Most field types (except multi-value) only use a single value.
    [[nodiscard]] std::string value() const;

    /// Returns the first value parsed as an integer.
    [[nodiscard]] int intValue() const;

    /// Returns the first value parsed as a double.
    [[nodiscard]] double doubleValue() const;

    /// Returns the first value parsed as a boolean.
    /// Treats "1", "true", and "on" as true; all other strings as false.
    [[nodiscard]] bool boolValue() const;
};


} // namespace smpl
} // namespace icy


/// @}
