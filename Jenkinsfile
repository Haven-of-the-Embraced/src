pipeline {
    agent {
        node {
            label 'master'
            customWorkspace '/home/jenkinsbuild/src'
        }
    }

    environment {
    DISCORD_WEBHOOK     = 'https://discord.com/api/webhooks/993588346955759740/JUkZ3EuJ-utk4O8MLxM-NlTfh8NWbsXOY79LtkEc2jZ_m3QIPJGnGgMOKyTFJLWBEay6'

    }
    
    stages {
        stage('Clean') {
            when {
                beforeAgent true
                expression { sh(returnStatus: true, script: 'git diff  origin/master --name-only | grep -E "*.h" > /dev/null') == 0 }
            }
            steps {
                echo 'Cleaning up object files...'
                sh 'make clean'
            }
        }
        stage('Build') {
            when {
                beforeAgent true
                anyOf {
                    expression { sh(returnStatus: true, script: 'git diff  origin/master --name-only | grep -E "*.h" > /dev/null') == 0 }
                    expression { sh(returnStatus: true, script: 'git diff  origin/master --name-only | grep -E "*.c" > /dev/null') == 0 }
                }
            }
            steps {
                echo 'Building MUD...'
                sh 'make'
            }
        }
        stage('Test') {
            when {
                beforeAgent true
                anyOf {
                    expression { sh(returnStatus: true, script: 'git diff  origin/master --name-only | grep -E "*.h" > /dev/null') == 0 }
                    expression { sh(returnStatus: true, script: 'git diff  origin/master --name-only | grep -E "*.c" > /dev/null') == 0 }
                }
            }
            steps {
                echo 'Executing a Test Run...'
                dir('/home/jenkinsbuild/area') {
                    sh '../src/rom 2005 testrun'
                }
            }
        }
    }
    post {
        changed {
            script {
                def branch = BRANCH_NAME
                def msg = "**Status:** " + currentBuild.currentResult.toLowerCase() + "\n"
                msg += "**Branch:** ${branch}\n"
                msg += "**Changes:** \n"
                if (!currentBuild.changeSets.isEmpty()) {
                    currentBuild.changeSets.first().getLogs().each {
                        msg += "- `" + it.getCommitId().substring(0, 8) + "` *" + it.getComment().substring(0, it.getComment().length()-1) + "*\n"
                    }
                }

                if (msg.length() > 1024) msg.take(msg.length() - 1024)

                discordSend thumbnail: "https://havenmud.net/images/taskfailed.jpg", successful: currentBuild.resultIsBetterOrEqualTo('SUCCESS'), description: "${msg}", link: env.BUILD_URL, title: "HavenMUD:${branch} #${BUILD_NUMBER}", webhookURL: "${DISCORD_WEBHOOK}"
            }
        }
    }
}
