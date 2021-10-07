import { jest } from '@jest/globals';
import { Signaling } from "../public/js/signaling";
import { waitFor } from "./testutils";

// eslint-disable-next-line jest/no-disabled-tests
describe.skip('signaling test in public mode on httpsignaling', () => {
  let signaling1;
  let signaling2;
  let signaling1DispachSpy;
  let signaling2DispachSpy;
  const connectionId1 = "12345";
  const connectionId2 = "67890";
  const testsdp = "test sdp";

  beforeEach(async () => {
    signaling1 = new Signaling();
    signaling1DispachSpy = jest.spyOn(signaling1, 'dispatchEvent');
    await signaling1.start();
    signaling2 = new Signaling();
    signaling2DispachSpy = jest.spyOn(signaling2, 'dispatchEvent');
    await signaling2.start();
  });

  afterEach(async () => {
    await signaling1.stop();
    signaling1DispachSpy.mockRestore();
    signaling1 = null;
    await signaling2.stop();
    signaling2DispachSpy.mockRestore();
    signaling2 = null;
  });

  test('onConnect', async () => {
    await signaling1.createConnection(connectionId1);
    expect(signaling1DispachSpy).toHaveBeenCalledWith(new CustomEvent("connect", { connectionId: connectionId1, polite: true }));

    await signaling1.deleteConnection(connectionId1);
    expect(signaling1DispachSpy).toHaveBeenCalledWith(new CustomEvent("disconnect", { connectionId: connectionId1 }));

  });

  test('onOffer', async () => {
    let offerRised;
    signaling2.addEventListener("offer", () => { offerRised = true; });

    await signaling1.createConnection(connectionId1);
    await signaling2.createConnection(connectionId2);
    expect(signaling2DispachSpy).toHaveBeenCalledWith(new CustomEvent("connect", { connectionId: connectionId2, polite: true }));
    expect(signaling1DispachSpy).toHaveBeenCalledWith(new CustomEvent("connect", { connectionId: connectionId1, polite: true }));

    await signaling1.sendOffer(connectionId1, testsdp);
    await waitFor(() => offerRised);

    await signaling1.deleteConnection(connectionId1);
    await signaling2.deleteConnection(connectionId2);
    expect(signaling2DispachSpy).toHaveBeenCalledWith(new CustomEvent("disconnect", { connectionId: connectionId2 }));
    expect(signaling1DispachSpy).toHaveBeenCalledWith(new CustomEvent("disconnect", { connectionId: connectionId1 }));
  });
});
