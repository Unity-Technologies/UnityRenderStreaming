module.exports = {
  env: {
    node: true,
    jest: true,
  },
  extends: ["plugin:@typescript-eslint/recommended"],
  parser: "@typescript-eslint/parser",
  parserOptions: {
    sourceType: "module",
    project: "./tsconfig.lint.json",
  },
  plugins: ["@typescript-eslint"],
  rules: {
    "@typescript-eslint/no-var-requires": "off"
  }
};
