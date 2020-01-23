## Adding Reviewers

Each request needs **at least 1 reviewer**. Each reviewer needs to approve the PR before it is merged. People from the team can add themselves if they would also like to review the changes. Reviewers can be removed if they have not yet started a review, but do not remove people who have added themselves.

For each reviewer added, **reach out to that person on Slack** so that they are made aware of the request.

As a Reviewer, please **comment on or approve a PR in a timely manner**. If you’re unable to do so, reach out to the author and let them know.

## Stale PRs

PRs without activity for several days should be **updated with comments or closed**. If the PR is waiting on something (e.g., another merge), close the PR and reopen it later. If the PR has yet to be reviewed by the reviewer(s), double check that they are the best person to review this PR.

## Guidelines

- Aim for not more than 200 to 400 lines of changes in a PR. Beyond that reviewers often stop trying to fully understand the code.
- Try to separate large automated-refactoring resulting in massive code changes from actual changes in seperate PRs.
- Release Notes must be updated when behaviour is changed

## Merging PRs

It’s preferred to **squash commits and merge** when completing a PR. To do so, click the dropdown arrow next to the merge button and select "Squash and merge". This will allow you to preview the new commit message and edit it.

Select another merge option if it’s required to preserve valuable information. For more information about the different merge options, see [GitHub’s documentation](https://help.github.com/articles/about-merge-methods-on-github/).

**Replace the merge commit message** with something meaning instead of using the default “merge pull request #123”.

When safe to do so, **delete your branch after merging**.

## All contributions are subject to the [Unity Contribution Agreement(UCA)](https://unity3d.com/legal/licenses/Unity_Contribution_Agreement)
By making a pull request, you are confirming agreement to the terms and conditions of the UCA, including that your Contributions are your original creation and that you have complete right and authority to make your Contributions.
