"use client";

import { Dashboard } from "@/components/ui/dashboard";
import { ChatbotUIContext } from "@/context/context";
import { convertBlobToBase64 } from "@/lib/blob-to-b64";
import { LLMID } from "@/types";
import { useParams, useRouter, useSearchParams } from "next/navigation";
import { ReactNode, useContext, useEffect, useState } from "react";
import Loading from "../loading";

import { getAssistantWorkspacesByWorkspaceId } from "@/app/phoenix_services/assistants";
import { getChatsByWorkspaceId } from "@/app/phoenix_services/chats";
import { getWorkspaceById } from "@/app/phoenix_services/workspace";
import { getCollectionsByWorkspaceId } from "@/app/phoenix_services/collections";
import { getFoldersByWorkspaceId } from "@/app/phoenix_services/folders";
import { getFilesByWorkspaceId } from "@/app/phoenix_services/files";
import { getPresetsByWorkspaceId } from "../../phoenix_services/presets";
import { getPromptsByWorkspaceId } from "@/app/phoenix_services/prompts";
import { getToolsByWorkspaceId } from "@/app/phoenix_services/tools";
import { getModelsByWorkspaceId } from "@/app/phoenix_services/models";
import { getAssistantImage } from "@/app/phoenix_services/assistant-images";

interface WorkspaceLayoutProps {
  children: ReactNode;
}

export default function WorkspaceLayout({ children }: WorkspaceLayoutProps) {
  const router = useRouter();
  const params = useParams();
  const searchParams = useSearchParams();
  const workspaceId = params.workspaceid as string;

  const {
    setChatSettings,
    setAssistants,
    setAssistantImages,
    setChats,
    setCollections,
    setFolders,
    setFiles,
    setPresets,
    setPrompts,
    setTools,
    setModels,
    selectedWorkspace,
    setSelectedWorkspace,
    setSelectedChat,
    setChatMessages,
    setUserInput,
    setIsGenerating,
    setFirstTokenReceived,
    setChatFiles,
    setChatImages,
    setNewMessageFiles,
    setNewMessageImages,
    setShowFilesDisplay,
  } = useContext(ChatbotUIContext);

  const [loading, setLoading] = useState(true);

  // Authentication check using access_token from localStorage
  useEffect(() => {
    (async () => {
      const token = localStorage.getItem("access_token");

      if (!token) {
        router.push("/login");
      } else {
        await fetchWorkspaceData(workspaceId);
      }
    })();
  }, []);

  useEffect(() => {
    (async () => await fetchWorkspaceData(workspaceId))();

    setUserInput("");
    setChatMessages([]);
    setSelectedChat(null);

    setIsGenerating(false);
    setFirstTokenReceived(false);

    setChatFiles([]);
    setChatImages([]);
    setNewMessageFiles([]);
    setNewMessageImages([]);
    setShowFilesDisplay(false);
  }, [workspaceId]);

  const fetchWorkspaceData = async (workspaceId: string) => {
    setLoading(true);

    try {
      // Fetch workspace data
      const workspace = await getWorkspaceById(workspaceId);
      setSelectedWorkspace(workspace);

      // Fetch assistants
      const assistants = await getAssistantWorkspacesByWorkspaceId(workspaceId);
      setAssistants(assistants);

      // Fetch and set assistant images
      // for (const assistant of assistants) {
      //   let url = '';
      //
      //   if (assistant.image_path) {
      //     url = await getAssistantImage(assistant.image_path);
      //   }
      //
      //   if (url) {
      //     const response = await fetch(url);
      //     const blob = await response.blob();
      //     const base64 = await convertBlobToBase64(blob);
      //
      //     setAssistantImages((prev) => [
      //       ...prev,
      //       {
      //         assistantId: assistant.id.toString(), // Convert id to string
      //         path: assistant.image_path,
      //         base64,
      //         url,
      //       },
      //     ]);
      //   } else {
      //     setAssistantImages((prev) => [
      //       ...prev,
      //       {
      //         assistantId: assistant.id.toString(), // Convert id to string
      //         path: assistant.image_path,
      //         base64: '',
      //         url: '',
      //       },
      //     ]);
      //   }
      // }


      // Fetch chats
      const chats = await getChatsByWorkspaceId(workspaceId);
      setChats(chats);

      // Fetch collections
      const collections = await getCollectionsByWorkspaceId(workspaceId);
      setCollections(collections);

      // Fetch folders
      const folders = await getFoldersByWorkspaceId(workspaceId);
      setFolders(folders);

      // Fetch files
      const files = await getFilesByWorkspaceId(workspaceId);
      setFiles(files);

      // Fetch presets
      const presets = await getPresetsByWorkspaceId(workspaceId);
      setPresets(presets);

      // Fetch prompts
      const prompts = await getPromptsByWorkspaceId(workspaceId);
      setPrompts(prompts);

      // Fetch tools
      const tools = await getToolsByWorkspaceId(workspaceId);
      setTools(tools);

      // Fetch models
      const models = await getModelsByWorkspaceId(workspaceId);
      setModels(models);

      // Set chat settings
      setChatSettings({
        model: (searchParams.get("model") ||
          workspace?.default_model ||
          "gpt-4-1106-preview") as LLMID,
        prompt:
          workspace?.default_prompt ||
          "You are a friendly, helpful AI assistant.",
        temperature: workspace?.default_temperature || 0.5,
        contextLength: workspace?.default_context_length || 4096,
        includeProfileContext: workspace?.include_profile_context ?? true,
        includeWorkspaceInstructions:
          workspace?.include_workspace_instructions ?? true,
        embeddingsProvider:
          (workspace?.embeddings_provider as "openai" | "local") || "openai",
      });
    } catch (error) {
      console.error("Error fetching workspace data:", error);
      // Handle error appropriately
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <Loading />;
  }

  return <Dashboard>{children}</Dashboard>;
}
