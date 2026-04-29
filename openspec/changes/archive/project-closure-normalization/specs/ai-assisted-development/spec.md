# AI-Assisted Development

## ADDED Requirements

### Requirement: Tool-Specific Instruction Files

THE Repository SHALL provide explicit instruction files for its primary AI coding tools.

#### Scenario: AI tool invoked

- **WHEN** an AI coding assistant operates in the repository
- **THEN** it can load project-specific guidance from the repo itself

### Requirement: Review Gate

THE Repository SHALL encourage `/review` or an equivalent review pass before merge for non-trivial work.

#### Scenario: Cleanup phase finished

- **WHEN** a significant implementation phase completes
- **THEN** the change goes through a review gate before merge
