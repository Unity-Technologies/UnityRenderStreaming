unityPath=$1
projectPath=$2
editorLogPath=$3
testResultPath=$4

echo Running Editor Tests
${unityPath}  -projectPath ${projectPath} -batchmode -runEditorTests -logFile ${editorLogPath} -editorTestsResultFile ${testResultPath}

