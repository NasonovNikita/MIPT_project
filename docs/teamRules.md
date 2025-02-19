# Branching Rules

## Main Branches
- `main`: The production-ready branch. Only tested and approved code should be merged here.
- `develop`: The integration branch for ongoing development. Feature branches are merged here before being promoted to `main`.

## Feature Branches
- **Naming**: `feature/feature-name` (e.g., `feature/user-authentication`).
- **Workflow**:
    1. Create a feature branch from `develop`.
    2. Commit changes to the feature branch.
    3. Submit a PR to merge into `develop`.
    4. After review and approval, merge the feature branch into `develop`.

## Bugfix Branches
- **Naming**: `bugfix/bug-description` (e.g., `bugfix/login-crash`).
- **Workflow**:
    1. Create a bugfix branch from `develop`.
    2. Commit fixes to the bugfix branch.
    3. Submit a PR to merge into `develop`.
    4. After review and approval, merge the bugfix branch into `develop`.

## Release Branches (not used yet)
- **Naming**: `release/version-number` (e.g., `release/v1.0.0`).
- **Workflow**:
    1. Create a release branch from `develop`.
    2. Perform final testing and bug fixes on the release branch.
    3. Merge the release branch into `main` and tag the release.
    4. Merge the release branch back into `develop` to include any last-minute fixes.

## Hotfix Branches
- **Naming**: `hotfix/bug-description` (e.g., `hotfix/security-patch`).
- **Workflow**:
    1. Create a hotfix branch from `main`.
    2. Commit fixes to the hotfix branch.
    3. Submit a PR to merge into `main`.
    4. Merge the hotfix branch into `develop` to keep it up to date.

## Additional Rules
- **Branch Protection**:
    - Protect `main` and `develop` branches:
        - Require PRs for merging.
        - Require approvals from at least one reviewer.
        - Require passing CI/CD checks (e.g., tests, linting).
- **Commit Messages**:
    - Use a consistent format (e.g., `type(scope): description`):
        - `feat(user-auth): add login functionality`
        - `fix(api): resolve null pointer exception`
        - `docs(readme): update installation instructions`
- **Cleanup**:
    - Delete branches after they are merged to keep the repository clean.<br>
        For example if some feature was accepted and pulled to `develop` it can be deleted.