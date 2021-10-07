export function waitFor(conditionFunction) {

  const poll = resolve => {
    if (conditionFunction()) resolve();
    else setTimeout(() => poll(resolve), 100);
  };

  return new Promise(poll);
}