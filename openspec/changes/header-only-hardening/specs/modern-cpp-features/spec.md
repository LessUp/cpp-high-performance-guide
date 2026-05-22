# Modern C++ Features

## ADDED Requirements

### Requirement: Header-Only Example Safety

THE Example_Module SHALL keep header-defined utilities safe to include from multiple translation units.

#### Scenario: Header-only examples link from multiple translation units

- **WHEN** the modern C++ example headers are included from more than one translation unit in the same target
- **THEN** the target links successfully without duplicate symbol errors

#### Scenario: Empty buffers are safe to observe

- **WHEN** the move-semantics helper functions process a default-constructed or zero-sized `Buffer`
- **THEN** they do not dereference invalid memory and complete without crashing
