// In Resolver.cpp, when processing an ImportNode

// Assume importedGlobals is the GlobalMap from the compiled imported file.

if (importNode->ImportedItemName.has_value()) {
    // --- Specific Item Import Logic ---
    std::string itemName = importNode->ImportedItemName.value();
    std::string newName = importNode->Alias.value_or(itemName);

    // This logic needs to be more robust, checking SpecificType as well
    if (importNode->Category == ImportCategory::Custom) {
        auto customDef = importedGlobals.GetCustom(itemName);
        if (customDef) m_CurrentGlobals.AddCustom(newName, customDef);
        else { /* Error: Item not found */ }
    } else if (importNode->Category == ImportCategory::Template) {
        auto templateDef = importedGlobals.GetTemplate(itemName);
        if (templateDef) m_CurrentGlobals.AddTemplate(newName, templateDef);
        else { /* Error: Item not found */ }
    }
    // ... other categories like Origin

} else {
    // --- Category or Full File Import Logic (as before) ---
    // ...
}