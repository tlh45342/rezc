 **VERSION String**:
   - Every project must include a `VERSION` string.
   - The format should be `X.Y.Z` (e.g., `1.0.0`).
   - Purpose: To track project iterations and updates.

2. **Directive Validation**:
   - All directives must be validated against a predefined list.
   - Invalid directives should trigger a clear error message.

3. **Endianness Handling**:
   - Default to little-endian, but support `.big-endian` where required.