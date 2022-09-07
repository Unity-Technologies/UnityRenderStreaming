import { MockSignaling, reset } from "./mocksignaling.js";
import { waitFor, getUniqueId, getRTCConfiguration } from "./testutils.js";
import { RenderStreaming } from "../src/renderstreaming.js";

describe.each([
  { mode: "private" },
  { mode: "public" }
])('renderstreaming test', ({ mode }) => {
  const connectionId1 = "12345";

  test(`createConnection in ${mode} mode`, async () => {
    reset(mode == "private");
    const config = getRTCConfiguration();
    const renderstreaming = new RenderStreaming(new MockSignaling(), config);
    await renderstreaming.start();

    let isConnect = false;
    renderstreaming.onConnect = () => isConnect = true;
    await renderstreaming.createConnection(connectionId1);
    await waitFor(() => isConnect);
    expect(isConnect).toBe(true);

    await renderstreaming.stop();
  });

  test(`addTrack in ${mode} mode`, async () => {
    reset(mode == "private");
    const config = getRTCConfiguration();
    const renderstreaming = new RenderStreaming(new MockSignaling(), config);
    await renderstreaming.start();

    let isConnect = false;
    renderstreaming.onConnect = () => isConnect = true;
    await renderstreaming.createConnection(connectionId1);
    await waitFor(() => isConnect);
    expect(isConnect).toBe(true);
    expect(renderstreaming.getTransceivers(connectionId1).length).toBe(0);

    const track = { id: getUniqueId(), kind: "audio" };
    renderstreaming.addTrack(track);
    expect(renderstreaming.getTransceivers(connectionId1).length).toBe(1);

    let isDisconnect = false;
    renderstreaming.onDisconnect = () => isDisconnect = true;
    await renderstreaming.deleteConnection();
    await waitFor(() => isDisconnect);
    expect(isDisconnect).toBe(true);

    await renderstreaming.stop();
  });

  test(`createChannel in ${mode} mode`, async () => {
    reset(mode == "private");
    const config = getRTCConfiguration();
    const renderstreaming = new RenderStreaming(new MockSignaling(), config);
    await renderstreaming.start();

    let isConnect = false;
    renderstreaming.onConnect = () => isConnect = true;
    await renderstreaming.createConnection(connectionId1);
    await waitFor(() => isConnect);
    expect(isConnect).toBe(true);
    expect(renderstreaming.getTransceivers(connectionId1).length).toBe(0);

    const label = "testlabel";
    const channel = renderstreaming.createDataChannel(label);
    expect(channel.label).toBe(label);

    let isDisconnect = false;
    renderstreaming.onDisconnect = () => isDisconnect = true;
    await renderstreaming.deleteConnection();
    await waitFor(() => isDisconnect);
    expect(isDisconnect).toBe(true);

    await renderstreaming.stop();
  });

  test(`onTrackEvent in ${mode} mode`, async () => {
    reset(mode == "private");

    const config = getRTCConfiguration();
    const renderstreaming1 = new RenderStreaming(new MockSignaling(), config);
    const renderstreaming2 = new RenderStreaming(new MockSignaling(), config);
    await renderstreaming1.start();
    await renderstreaming2.start();

    let isConnect1 = false;
    renderstreaming1.onConnect = () => isConnect1 = true;
    let isConnect2 = false;
    renderstreaming2.onConnect = () => isConnect2 = true;

    await renderstreaming1.createConnection(connectionId1);
    await renderstreaming2.createConnection(connectionId1);
    await waitFor(() => isConnect1 && isConnect2);
    expect(isConnect1).toBe(true);
    expect(isConnect2).toBe(true);

    let isGotOffer1 = false;
    let isOnTrack1 = false;
    let isGotAnswer2 = false;
    renderstreaming1.onGotOffer = () => { isGotOffer1 = true; };
    renderstreaming1.onTrackEvent = () => { isOnTrack1 = true; };
    renderstreaming2.onGotAnswer = () => { isGotAnswer2 = true; };

    expect(renderstreaming1.getTransceivers(connectionId1).length).toBe(0);

    const track = { id: getUniqueId(), kind: "audio" };
    renderstreaming2.addTrack(track);
    expect(renderstreaming2.getTransceivers(connectionId1).length).toBe(1);
    await waitFor(() => isGotOffer1);
    expect(isGotOffer1).toBe(true);

    await waitFor(() => isOnTrack1);
    expect(isOnTrack1).toBe(true);
    expect(renderstreaming1.getTransceivers(connectionId1).length).toBe(1);

    await waitFor(() => isGotAnswer2);
    expect(isGotAnswer2).toBe(true);

    let isDisconnect1 = false;
    renderstreaming1.onDisconnect = () => isDisconnect1 = true;
    let isDisconnect2 = false;
    renderstreaming2.onDisconnect = () => isDisconnect2 = true;

    await renderstreaming1.deleteConnection();
    await renderstreaming2.deleteConnection();
    await waitFor(() => isDisconnect1 && isDisconnect2);
    expect(isDisconnect1).toBe(true);
    expect(isDisconnect2).toBe(true);

    await renderstreaming1.stop();
    await renderstreaming2.stop();
  });

  test(`onAddDataChannel in ${mode} mode`, async () => {
    reset(mode == "private");

    const config = getRTCConfiguration();
    const renderstreaming1 = new RenderStreaming(new MockSignaling(), config);
    const renderstreaming2 = new RenderStreaming(new MockSignaling(), config);
    await renderstreaming1.start();
    await renderstreaming2.start();

    let isConnect1 = false;
    renderstreaming1.onConnect = () => isConnect1 = true;
    let isConnect2 = false;
    renderstreaming2.onConnect = () => isConnect2 = true;

    await renderstreaming1.createConnection(connectionId1);
    await renderstreaming2.createConnection(connectionId1);
    await waitFor(() => isConnect1 && isConnect2);
    expect(isConnect1).toBe(true);
    expect(isConnect2).toBe(true);

    let isGotOffer1 = false;
    let isAddChannel1 = false;
    let isGotAnswer2 = false;
    renderstreaming1.onGotOffer = () => { isGotOffer1 = true; };
    renderstreaming1.onAddChannel = () => { isAddChannel1 = true; };
    renderstreaming2.onGotAnswer = () => { isGotAnswer2 = true; };

    renderstreaming2.createDataChannel("testchannel");
    await waitFor(() => isGotOffer1);
    expect(isGotOffer1).toBe(true);

    await waitFor(() => isAddChannel1);
    expect(isAddChannel1).toBe(true);

    await waitFor(() => isGotAnswer2);
    expect(isGotAnswer2).toBe(true);

    let isDisconnect1 = false;
    renderstreaming1.onDisconnect = () => isDisconnect1 = true;
    let isDisconnect2 = false;
    renderstreaming2.onDisconnect = () => isDisconnect2 = true;

    await renderstreaming1.deleteConnection();
    await renderstreaming2.deleteConnection();
    await waitFor(() => isDisconnect1 && isDisconnect2);
    expect(isDisconnect1).toBe(true);
    expect(isDisconnect2).toBe(true);

    await renderstreaming1.stop();
    await renderstreaming2.stop();
  });

});
