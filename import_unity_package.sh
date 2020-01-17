unityPath=$1
projectPath=$2
packagePath=$3
editorLogPath=$4

echo Importing Package: ${packagePath}
${unityPath} -projectPath ${projectPath} -quit -batchmode -importPackage ${packagePath} -logFile ${editorLogPath}