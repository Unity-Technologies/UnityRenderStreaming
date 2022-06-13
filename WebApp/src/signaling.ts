import * as express from 'express';
import { v4 as uuid } from 'uuid';
import * as handler from'./class/httphandler';

const router: express.Router = express.Router();
router.use(handler.checkSessionId);
router.get('/connection', handler.getConnection);
router.get('/offer', handler.getOffer);
router.get('/answer', handler.getAnswer);
router.get('/candidate', handler.getCandidate);
router.put('', (req: express.Request, res: express.Response) => {
    const sessionId = uuid();
    handler.createSession(sessionId, res);
});
router.delete('', handler.deleteSession);
router.put('/connection', handler.createConnection);
router.delete('/connection', handler.deleteConnection);
router.post('/offer', handler.postOffer);
router.post('/answer', handler.postAnswer);
router.post('/candidate', handler.postCandidate);

export default router;
